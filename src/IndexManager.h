#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H

#include <stdio.h>
#include "BasicManager.h"
#include <map>

#define M 4

struct BPlusTree {
	//这里开的M其实比需要的大了1，是为了方便写split（暂时存一下再split）
	Element key[M];
	BPlusTree *son[M + 1], *fa, *Left, *Right;
	int pos[M];
	int num, isleaf, isroot;
	int id;  //记录这个点的编号
	static int tot;
	BPlusTree() {
		num = 0; id = ++tot;
		for (int i = 0; i <= M; i++) son[i] = nullptr;
		fa = Left = Right = nullptr;
		isleaf = isroot = 0;
	}
};

extern map<string, BPlusTree*>rt;
extern string rtIndexName;

void ReadIndex();
void SaveIndex();
void LoadTree(string name);
void SaveNode(map<string, FILE*>::iterator out, BPlusTree *cur);
void SaveTree(string name);
void SaveAllTree();
void Free(BPlusTree *cur);
int CalcSize(BPlusTree *cur);
int FindLeaf(BPlusTree *cur, Element key);
int Position(BPlusTree *cur, Element key);
int GetID(BPlusTree *cur, BPlusTree *son);
void SimpleInsert(BPlusTree *cur, int k, Element key, BPlusTree *son, int offset);
void SimpleDelete(BPlusTree *cur, int k);
void InsertNode(BPlusTree *cur, Element key, BPlusTree *son);
int InsertLeaf(BPlusTree *cur, Element key, int offset);
void Split(BPlusTree *cur);
void DeleteLeaf(BPlusTree *cur, Element key);
void DeleteNode(BPlusTree *cur, int k);
void PrintNode(BPlusTree *cur);
void PrintTree(BPlusTree *cur);

#endif 