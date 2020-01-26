#include<bits/stdc++.h>
using namespace std;
int main(){
	freopen("test8.sql", "w", stdout);
	printf("create table t2(id int, id2 int unique, primary key(id));\n");
	//printf("create index work2 on t2(id2)\n");
	for (int i = 1; i <= 400; i++)
		printf("insert into t2 values(%d, %d);\n", i, i);
	
	freopen("test9.sql", "w", stdout);
	printf("create table t3(id int, id2 int unique, primary key(id));\n");
	printf("create index work3 on t3(id2)\n");
	for (int i = 1; i <= 20000; i++)
		printf("insert into t3 values(%d, %d);\n", i, i);
}
