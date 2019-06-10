#ifndef BASICMANAGER_H
#define BASICMANAGER_H
#include <string>
#include <cstring>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
using namespace std;

#define NONE 0

#define LESS 1
#define MORE 2
#define LESSQ 3
#define MOREQ 4
#define EQUAL 5
#define NOTEQUAL 6

#define DATASIZE 256

#define INT 1
#define FLOAT 2
#define CHAR 3

struct InputData {
	char tp, uni, key, ind; int len; char name[248];
	InputData() { tp = uni = key = ind = 0; len = 255; memset(name, 0, sizeof(name)); }
	InputData(string _name, int _tp, int _uni, int _key, int _ind, int _len){ 
		strcpy(name, _name.c_str());
		tp = _tp, uni = _uni | _key;
		key = _key, ind = _ind, len = _len;
	}
};
struct Element {
	char tp; char d[255];
	int operator < (const Element &b)const {
		return tp == CHAR ? strcmp(d, b.d) <  0 : (tp == INT ? atoi(d) < atoi(b.d) : atof(d) < atof(b.d));
	}
	int operator > (const Element &b)const {
		return tp == CHAR ? strcmp(d, b.d) >  0 : (tp == INT ? atoi(d) > atoi(b.d) : atof(d) > atof(b.d));
	}
	int operator <= (const Element &b)const {
		return tp == CHAR ? strcmp(d, b.d) <= 0 : (tp == INT ? atoi(d) <= atoi(b.d) : atof(d) <= atof(b.d));
	}
	int operator >= (const Element &b)const {
		return tp == CHAR ? strcmp(d, b.d) >= 0 : (tp == INT ? atoi(d) >= atoi(b.d) : atof(d) >= atof(b.d));
	}
	int operator == (const Element &b)const {
		return tp == CHAR ? strcmp(d, b.d) == 0 : (tp == INT ? atoi(d) == atoi(b.d) : atof(d) == atof(b.d));
	}
	int operator != (const Element &b)const {
		return tp == CHAR ? strcmp(d, b.d) != 0 : (tp == INT ? atoi(d) != atoi(b.d) : atof(d) != atof(b.d));
	}
	Element() { memset(d, 0, sizeof(d)); tp = 0; }
	Element(int _tp, const char *s) { memset(d, 0, sizeof(d)); strcpy(d, s); tp = _tp; }
};
struct Condition {
	string name;
	int op;
	Element p;
	Condition() {};
	Condition(string _name, int _op, Element _p) {
		name = _name;op = _op;p = _p;
	}
	void Set(string _name, string _op, Element _p) {
		name = _name; p = _p;
		if (_op == "<") op = LESS;
		else if (_op == ">") op = MORE;
		else if (_op == "<=") op = LESSQ;
		else if (_op == ">=") op = MOREQ;
		else if (_op == "=") op = EQUAL;
		else if (_op == "!=") op = NOTEQUAL;
		else op = NONE;
	}
};

#endif