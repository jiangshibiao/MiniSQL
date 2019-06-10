#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H

#include <stdio.h>

#define M 4

struct Element{
	int d;
	int operator < (const Element &b)const{
		return d < b.d;
	}
	int operator > (const Element &b)const{
		return d > b.d;
	}
	int operator >= (const Element &b)const{
		return d >= b.d;
	}
	int operator == (const Element &b)const{
		return d == b.d;
	}
	int operator != (const Element &b)const{
		return d != b.d;
	}
};

struct BPlusTree {
	//���￪��M��ʵ����Ҫ�Ĵ���1����Ϊ�˷���дsplit����ʱ��һ����split��
	Element key[M];
	BPlusTree *son[M + 1], *fa, *Left, *Right;
	int pos[M];
	int num, isleaf, isroot;
	int id;  //������ļ�ʱ����¼�����ı��
	static int tot;
	BPlusTree() {
		num = 0; id = ++tot;
		for (int i = 0; i <= M; i++) son[i] = nullptr;
		fa = Left = Right = nullptr;
		isleaf = isroot = 0;
	}
};
int BPlusTree::tot = 0;

BPlusTree *Locate(BPlusTree *cur, Element key);
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
