#ifndef CATALOGMANAGER_H
#define CATALOGMANAGER_H

#include <string>
#include <vector>
#include "BufferManager.h"
#include "BasicManager.h"
using namespace std;

class Catalog {
public:
	Catalog() {};
	~Catalog() {};
	void ReadTable();
	vector<InputData> GetTable(string TableName);
	int ExistTable(string TableName);
	void CreateTable(string TableName, vector<InputData>Table);
	int DropTable(string TableName);
};
#endif