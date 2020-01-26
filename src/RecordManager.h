#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H
#include "BufferManager.h"
#include "IndexManager.h"
#include <string>
#include <vector>
using namespace std;
class Record{
public:
	BFM *bfm;
	Record() { bfm = new BFM(); }
	~Record() { delete bfm; }
	int Insert(string TableName, vector<Element>Data);
	int Delete(string TableName, vector<InputData>Table, vector<Condition>Cond);
	int FindRecord(string TableName, vector<InputData>Table, vector<Condition>Cond, int visual, int ins);
//private:
	//Element LoadData(bufferIter it, int pos);
	//void SetData(bufferIter it, int pos, Element val);
};
#endif