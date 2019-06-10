#include "Interpreter.h"
#include <stdio.h>

string Interpreter::lstrip(string cur) {
	while (cur.size() && (cur[0] == ' ' || cur[0] == '\t'))
		cur.erase(cur.begin());
	return cur;
}
string Interpreter::rstrip(string cur) {
	while (cur.size() && (cur.back() == ' ' || cur.back() == '\t'))
		cur.erase(--cur.end());
	return cur;
}

string Interpreter::strip(string cur) {
	return rstrip(lstrip(cur));
}

string Interpreter::reduce(string cur) {
	string ret = "";
	for (auto t : cur) if (t != ' ' && t != '\t') ret += t;
	return ret;
}

string Interpreter::TryCreateTable(string cur) {
	if (cur.back() != ')') return "Fail! Find no right bracket.";
	cur.erase(--cur.end());
	int LeftBracket = cur.find('(');
	if (LeftBracket == -1) return "Fail! Find no left bracket.";
	if (LeftBracket == 0)  return "Fail! Miss the table name.";
	string TableName = strip(cur.substr(0, LeftBracket));
	cur = cur.substr(LeftBracket + 1);
	if (cur == "") return "Fail! Table can't be empty.";
	vector<InputData>Table;
	while (true) {
		int comma = cur.find(',');
		if (comma == -1) break;
		string block = strip(cur.substr(0, comma));
		cur = strip(cur.substr(comma + 1));
		InputData now;
		if (block.size() > 6 && block.substr(block.size() - 6) == "unique")
			now.uni = 1, block = strip(block.substr(0, block.size() - 6));
		int space = block.find(' ');
		if (space == -1) return "Fail! Find no spacce to split type and name.";
		strcpy(now.name, strip(block.substr(0, space)).c_str());

		string type = reduce(block.substr(space + 1));
		if (type == "int") now.tp = INT;
		else if (type == "float") now.tp = FLOAT;
		else if (type.substr(0, 4) == "char" && type.substr(4, 1) == "(" && type.back() == ')') {
			try {
				int number = stoi(type.substr(5, type.length() - 6));
				if (number >= 1 && number <= 255)
					now.tp = CHAR, now.len = number;
				else return "Fail! The number in char() is out of range.";
			}
			catch (...) {
				return "Fail! The number in char() can't be recognized.";
			}
		}
		else return "Fail! No such type like [" + type + "].";
		for (auto t : Table)
			if (strcmp(t.name, now.name) == 0) 
				return "Fail! The names of Attributes are duplicated."; 
		Table.push_back(now);
	}
	if (Table.size() > MAXELEMENT) return "Fail! The number of Attributes exceeds.";
	if (cur.substr(0, 7) != "primary") return "Fail! The format of [primary key] is wrong.";
	cur = strip(cur.substr(7));
	if (cur.substr(0, 3) != "key")	   return "Fail! The format of [primary key] is wrong.";
	cur = strip(cur.substr(3));
	if (!cur.size() || cur[0] != '(' || cur.back() != ')')
		return "Fail! The bracket of primary key is not correct.";
	cur = strip(cur.substr(1, cur.length() - 2));
	int Findkey = 0;
	for (auto &t : Table)
		if ((string)t.name == cur) {
			t.uni = t.key = 1;
			Findkey = 1;
		}
	if (!Findkey) return "Fail! primary key [" + cur + "] is not in the table.";
	return api->CreateTable(TableName, Table);
}

string Interpreter::TryDropTable(string cur) {
	return api->DropTable(cur);
}

string Interpreter::TryCreateIndex(string cur) {
	int space = cur.find(' ');
	if (space == -1) return "Can't recongnize the index name.";
	string name = cur.substr(0, space);
	cur = strip(cur.substr(space + 1));
	if (cur.substr(0, 2) != "on") return "Miss the key word [on].";
	cur = strip(cur.substr(2));
	int Left = cur.find('(');
	int Right = cur.find(')', Left + 1);
	if (Left == -1 || Right == -1) return "The bracket is not correct.";
	return api->CreateIndex(cur.substr(0, Left), cur.substr(Left + 1, Right - Left - 1), name);
}

string Interpreter::TryDropIndex(string cur) {
	return api->DropIndex(cur, 0);
}

string Interpreter::TryInsert(string cur) {
	if (cur.substr(0, 4) != "into") return "Can't find the key word [into].";
	cur = strip(cur.substr(4));
	int space = cur.find(' ');
	if (space == -1) return "The format of insertion is wrong.";
	string TableName = cur.substr(0, space);
	if (!api->CM->ExistTable(TableName)) return "Fail! Table [" + TableName + "] hasn't existed yet!";
	cur = strip(cur.substr(space + 1));
	if (cur.substr(0, 6) != "values") return "Fail! Can't find the key word [values].";
	cur = strip(cur.substr(6));
	if (!cur.size() || cur[0] != '(' || cur.back() != ')')
		return "Fail! The bracket of values is not correct.";
	cur = strip(cur.substr(1, cur.length() - 2));
	vector<string>block;
	vector<Element>vec;
	while (true) {
		int comma = cur.find(',');
		if (comma == -1) break;
		block.push_back(strip(cur.substr(0, comma)));
		cur = strip(cur.substr(comma + 1));
	}
	block.push_back(cur);

	vector<InputData>Table = api->CM->GetTable(TableName);
	if (block.size() != Table.size()) return "Fail! The number of data you insert is not correct.";
	
	for (int i = 0; i < block.size(); i++) {
		if (!block[i].size())
			return "Fail! Some attribute is empty.";
		string typeerror = "Fail! The type of [" + block[i] + "] is not correct.";
		if (block[i][0] == '\'' || block[i][0] == '\"') {
			if (block[i].back() != block[i][0]) return "Fail! The quotes of [" + block[i] + "] is not correct.";
			if (Table[i].tp != CHAR) return typeerror;
			block[i] = block[i].substr(1, block[i].length() - 2);
		}
		else {
			if (Table[i].tp == CHAR) return typeerror;
			try {
				if (Table[i].tp == INT) stoi(block[i]); else stof(block[i]);
			}
			catch (...) {
				return typeerror;
			}
		}
		Element tmp = Element(Table[i].tp, block[i].c_str());
		vec.push_back(tmp);
	}
	return api->Insert(TableName, vec);
}

string Interpreter::GetCondition(string cur, vector<InputData>Table, Condition &ret) {
	if (cur == "") return "Fail! Some condition is null.";
	string error = "Fail! The expression [" + cur + "] is invalid.";
	int t = -1;
	for (int i = 0; i < cur.length(); i++)
		if (cur[i] == '<' || cur[i] == '>' || cur[i] == '=' || cur[i] == '!') {
			t = i;
			break;
		}
	if (t == -1 || t == cur.length() - 1) return error;
	string name = strip(cur.substr(0, t));
	string opt = cur.substr(t, 1 + (cur[t + 1] == '='));
	string val = strip(cur.substr(t + 1 + (cur[t + 1] == '=')));
	int type = NONE;
	for (int i = 0; i < Table.size(); i++)
		if (Table[i].name == name) 
			type = Table[i].tp;
	if (type == NONE) return "Fail! Attribute [" + name + "] is not in the table.";
	
	if (val == "") return error;
	if (val[0] == '\'' || val[0] == '\"') {
		if (val.back() != val[0] || type != CHAR) return error;
		val = val.substr(1, val.length() - 2);
	}
	else {
		if (type == CHAR) return error;
		try {
			if (type == INT) stoi(val); else stof(val);
		}
		catch (...) {
			return error;
		}
	}
	Element data = Element(type, val.c_str());
	ret.Set(name, opt, data);
	if (ret.op == NONE) return error;
	return "";
}

vector<Condition> Interpreter::FindCondition(string TableName, string cur, string &error) {
	vector<Condition>ret; error = "";
	if (!api->CM->ExistTable(TableName)) 
		return error = "Fail! Table [" + TableName + "] hasn't existed yet!", ret;
	if (cur == "") return ret;
	vector<InputData>Table = api->CM->GetTable(TableName);
	if (cur.substr(0, 5) != "where") return error = "Fail! Can't find the key word [where].", ret;
	cur = strip(cur.substr(5));
	Condition Cond; int cnt = 0;
	while (true) {
		int pos = cur.find("and");
		if (pos == -1) break;
		error = GetCondition(cur.substr(0, pos), Table, Cond);
		ret.push_back(Cond); if (error != "") return ret;
		cur = strip(cur.substr(pos + 3));
	}
	error = GetCondition(cur, Table, Cond);
	ret.push_back(Cond); return ret;
}

string Interpreter::TrySelect(string cur) {
	if (!cur.size() || cur[0] != '*')
		return "Fail! Miss the key word [*]";
	cur = strip(cur.substr(1));
	if (cur.size() < 4 || cur.substr(0, 4) != "from")
		return "Fail! Miss the key word [from].";
	cur = strip(cur.substr(4));
	int space = cur.find(' ');
	if (space == -1) space = cur.length();
	string TableName = cur.substr(0, space);
	cur = (space + 1 < cur.size()) ? strip(cur.substr(space + 1)) : "";
	string error;
	vector<Condition>Cond = FindCondition(TableName, cur, error);
	if (error != "") return error;
	return api->Select(TableName, Cond);
}


string Interpreter::TryDelete(string cur) {
	if (cur.size() < 4 || cur.substr(0, 4) != "from")
		return "Fail! Miss the key word [from].";
	cur = strip(cur.substr(4));
	int space = cur.find(' ');
	if (space == -1) space = cur.length();
	string TableName = cur.substr(0, space);
	cur = (space + 1 < cur.size()) ? strip(cur.substr(space + 1)) : "";
	string error;
	vector<Condition>Cond = FindCondition(TableName, cur, error);
	return (error != "") ? error : api->Delete(TableName, Cond);
}

string Interpreter::check(string cur, int canFile = 1) {
	cur = strip(cur);
	if (cur.substr(0, 6) == "create") {
		cur = strip(cur.substr(6));
		if (cur.substr(0, 5) == "table")
			return TryCreateTable(strip(cur.substr(5)));
		if (cur.substr(0, 5) == "index")
			return TryCreateIndex(strip(cur.substr(5)));
	}
	else if (cur.substr(0, 4) == "drop") {
		cur = strip(cur.substr(4));
		if (cur.substr(0, 5) == "table")
			return TryDropTable(strip(cur.substr(5)));
		if (cur.substr(0, 5) == "index")
			return TryDropIndex(strip(cur.substr(5)));
	}
	else if (cur.substr(0, 6) == "insert")
		return TryInsert(strip(cur.substr(6)));
	else if (cur.substr(0, 6) == "delete")
		return TryDelete(strip(cur.substr(6)));
	else if (cur.substr(0, 6) == "select")
		return TrySelect(strip(cur.substr(6)));
	else if (cur.substr(0, 8) == "execfile") {
		if (canFile) return SolveFile(strip(cur.substr(8)));
		return "You can't execute file in the file! This instruction will be ignored.";
	}
	return "Can't recongnize your instruction!";
}

string Interpreter::SolveFile(string cur) {
	FILE *in = fopen(cur.c_str(), "r");
	if (in == NULL) return "No such file to execute!";
	int number = 0;
	while (true) {
		string Command = "";
		char ch = ' ';
		int getCommand = 0;
		while (true) {
			while (true) {
				ch = fgetc(in);
				if (ch == '\n' || ch == EOF) break;
				Command += ch;
			}
			int ed = Command.find(';');
			if (ed != -1) {
				Command = Command.substr(0, ed);
				getCommand = 1;
				break;
			}
			if (ch == EOF) break;
		}
		if (!getCommand || Command.substr(0, 4) == "quit") break;
		string Message = Command.substr(0, 4) == "save" ? Save(), "Save successfully." : check(Command, 0);
		if (Message != "") {
			++number;
			if (number == MAXNUMBER)
				printf("The number of instructions is too large, the rest results will be hidden.\n");
			else if (number < MAXNUMBER)
				printf("%s\n", Message.c_str());
		}
		if (ch == EOF) break;
	}
	return "Execute file [" + cur + "] successfully!";
}

void Interpreter::Save() {
	BFM::BufferManagerFlush();
	SaveIndex();
	SaveAllTree();
	for (auto t : File)
		fclose(t.second);
	File.clear();
	Load();
}

void Interpreter::Load() {
	ReadIndex();
	api->CM->ReadTable();
}


int main() {
	Interpreter *cur = new Interpreter();
	cur->Load();
	while (true) {
		string Command = "";
		printf("minisql-->");
		while (true) {
			while (true) {
				char ch = getchar();
				if (ch == '\n') break;
				Command += ch;
			}
			int ed = Command.find(';');
			if (ed != -1) {
				Command = Command.substr(0, ed);
				break;
			}
			printf("       -->");
		}
		if (Command.substr(0, 4) == "quit") break;
		int Time1 = clock();
		if (Command.substr(0, 4) == "save") {
			cur->Save(); 
			printf("Save successfully.\n");
			int Time2 = clock();
			printf("Time cost : %.4fs\n", (Time2 - Time1) * 1.0 / CLOCKS_PER_SEC);
			continue;
		}
		string Message = cur->check(Command);
		if (Message != "")
			printf("%s\n", Message.c_str());
		int Time2 = clock();
		printf("Time cost : %.4fs\n", (Time2 - Time1) * 1.0 / CLOCKS_PER_SEC);
	}
	cur->Save();
	return 0;
}