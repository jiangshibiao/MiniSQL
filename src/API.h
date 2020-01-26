#ifndef API_H
#define API_H

#include <string>
#include <cstring>
#include <vector>
#include <stdio.h>
#include "IndexManager.h"
#include "CatalogManager.h"
#include "BufferManager.h"
#include "RecordManager.h"

class API{
public:
	Catalog *CM;
	Record  *RM;
	API() { CM = new Catalog(); RM = new Record(); }
	~API() { delete CM; delete RM; }
    
	string DropTable (string TableName);
    string CreateTable(string TableName, vector<InputData> Table);
	string CreateIndex(string TableName, string AttributeName, string IndexName);
	string DropIndex(string IndexName, int compulsory);
	string Insert(string TableName, vector<Element>vec);
	string Delete(string TableName, vector<Condition>Cond);
	string QuickSelect(string TableName, vector<InputData>Table, string name, Element key);
	string Select(string TableName, vector<Condition>Cond);
   
private:

};
#endif