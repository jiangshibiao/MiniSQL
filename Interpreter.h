#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <vector>
#include "API.h"
class Interpreter{
public:
	API *api;
	Interpreter() { api = new API(); }
	~Interpreter(){}
	string check(string cur, int canFile);
	string SolveFile(string cur);
	void Save();
	void Load();
private:
	string lstrip(string cur);
	string rstrip(string cur);
	string  strip(string cur);
	string reduce(string cur);
	
	string TryCreateTable(string cur);
	string TryDropTable(string cur);
	string TryInsert(string cur);
	string TryCreateIndex(string cur);
	string TryDropIndex(string cur);

	string GetCondition(string cur, vector<InputData>Table, Condition &ret);
	vector<Condition> FindCondition(string TableName, string cur, string &error);
	string TryDelete(string cur);
	string TrySelect(string cur);
};

#endif