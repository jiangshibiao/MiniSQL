#include "API.h"

string API::CreateTable(string TableName, vector<InputData>Table) {
	if (CM->ExistTable(TableName))
		return "Fail! Table [" + TableName + "] has been created yet.";
	CM->CreateTable(TableName, Table);
	CreateFile(TableName, 0);
	for (int i = 0; i < Table.size(); i++)
		if (Table[i].key) 
			CreateIndex(TableName, Table[i].name, TableName + "__" + Table[i].name);
	return "Create Table [" + TableName + "] successfully."; 
}

string API::DropTable(string TableName) {
	if (!CM->ExistTable(TableName))
		return "Table [" + TableName + "] doesn't exist.";
	BFM::deleteFile(TableName);
	DeleteFile(TableName);
	
	CM->DropTable(TableName);
	
	while (true) {
		auto it = IndexID1.lower_bound(make_pair(TableName, ""));
		if (it == IndexID1.end() || (it->first).first != TableName) break;
		DropIndex(it->second, 1);
	}

	return "Drop Table [" + TableName + "] successfully.";
}

string API::CreateIndex(string TableName, string AttributeName, string name) {
	if (!CM->ExistTable(TableName))
		return "Fail! Table [" + TableName + "] hasn't been created yet.";
	vector<InputData>Table = CM->GetTable(TableName);
	int k = -1;
	for (int i = 0; i < Table.size(); i++) 
		if (Table[i].name == AttributeName) k = i;
	if (k == -1) return "Fail! Can't find [" + AttributeName + "] in the table.";
	if (Table[k].uni == 0) return "Fail! The Attribute [" + AttributeName + "] is not unique.";
	if (IndexID1.find(make_pair(TableName, AttributeName)) != IndexID1.end()) {
		if (Table[k].key)
			return "Fail! The index of primary key has been created by default.";
		else
			return "Fail! The attribute [" + AttributeName + "] has created the index yet.";
	}
	if (IndexID2.find(name) != IndexID2.end())
		return "Fail! Index name [" + name + "] is used.";
	IndexID1[make_pair(TableName, AttributeName)] = name;
	IndexID2[name] = make_pair(TableName, AttributeName);

	CreateFile(name, 0);
	
	BPlusTree::tot = 0;
	BPlusTree *cur = new BPlusTree();
	cur->isroot = cur->isleaf = 1; cur->num = 0;
	rt[rtIndexName = name] = cur;

	vector<Condition>Null;
	RM->FindRecord(TableName, Table, Null, 0, 1);

	return "Index [" + name + "] is created successfully.";
}

string API::DropIndex(string name, int compulsory = 1) {
	auto it = IndexID2.find(name);
	if (it == IndexID2.end()) return "Can't find index [" + name + "].";
	pair<string, string>tmp = it->second;
	if (!compulsory && name == (it->second).first + "__" + (it->second).second)
		return "The index of primary key can't be dropped.";
	Free(rt[it->first]);
	rt.erase(it->first);
	IndexID1.erase(tmp);
	IndexID2.erase(it);
	DeleteFile(name);
	return "Index [" + name + "] is dropped successfully.";
}

string API::Insert(string TableName, vector<Element>vec) {
	vector<InputData>Table = CM->GetTable(TableName);
	for (int i = 0; i < Table.size(); i++){
		if (strlen(vec[i].d) > Table[i].len || strlen(vec[i].d) == 0)
			return "Fail! The length of [" + (string)Table[i].name + "] is out of range!";

		auto it = IndexID1.find(make_pair(TableName, Table[i].name));
		if (it != IndexID1.end()) {
			string name = it->second;
			int ret = FindLeaf(rt[rtIndexName = name], vec[i]);
			if (ret >=0 ) return "Fail! Unique value [" + (string)Table[i].name + "] has existed yet.";
		}
		else if (Table[i].uni) {
			vector<Condition>Cond;
			Condition tmp = Condition(Table[i].name, EQUAL, vec[i]);
			Cond.push_back(tmp);
			if (RM->FindRecord(TableName, Table, Cond, 0, 0))
				return "Fail! Unique value [" + (string)Table[i].name + "] has existed yet.";
		}
	}
	//printf("??? %d %s %d\n", vec.size(), vec.back().d, memory);
	int offset = RM->Insert(TableName, vec);
	for (int i = 0; i < Table.size(); i++) {
		auto it = IndexID1.find(make_pair(TableName, Table[i].name));
		if (it == IndexID1.end()) continue;
		rtIndexName = it->second;
		InsertLeaf(rt[rtIndexName], vec[i], offset);
	}
	return "Insert Values into [" + TableName + "] successfully.";
}

string API::QuickSelect(string TableName, vector<InputData>Table, string name, Element key) {
	int pos = FindLeaf(rt[name], key);
	if (pos == -1) return "Find no records.";
	printf("The result is:\n");
	for (auto k : Table)
		printf("%12s", k.name);
	printf("\n");
	bufferIter it = BFM::BufferManagerRead(TableName.c_str(), pos, Table.size());
	int go = 0;
	for (auto k : Table) {
		Element ret = BFM::LoadData(it, go);
		go += DATASIZE;
		printf("%12s", ret.d);
	}
	printf("\n"); return "";
}

string API::Select(string TableName, vector<Condition>Cond) {
	if (!CM->ExistTable(TableName))
		return "Fail! Table " + TableName + " doesn't exist yet!";
	vector<InputData>Table = CM->GetTable(TableName);
	if (Cond.size() == 1 && Cond[0].op == EQUAL) {
		auto it = IndexID1.find(make_pair(TableName, Cond[0].name));
		if (it != IndexID1.end()) return QuickSelect(TableName, Table, it->second, Cond[0].p);
	}
	int ret = RM->FindRecord(TableName, Table, Cond, 1, 0);
	return ret <= 0 ? "Find no records." : "";
}

string API::Delete(string TableName, vector<Condition>Cond) {
	if (!CM->ExistTable(TableName))
		return "Fail! Table [" + TableName + "] doesn't exist yet!";
	
	vector<InputData>Table = CM->GetTable(TableName);
	int delnum = 0;
	if (Cond.size() == 1 && Cond[0].op == EQUAL) {
		auto it = IndexID1.find(make_pair(TableName, Cond[0].name));
		string name = it->second;
		delnum = FindLeaf(rt[rtIndexName = name], Cond[0].p);
		if (delnum > -1) DeleteLeaf(rt[rtIndexName = name], Cond[0].p), delnum = 1; else delnum = 0;
	}
	else delnum = RM->Delete(TableName, Table, Cond);
	return "Delete " + to_string(delnum) + " records successfully.";
}