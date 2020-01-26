#include "IndexManager.h"

map<string, BPlusTree*>rt;
string rtIndexName;
int BPlusTree::tot = 0;

void ReadIndex() {
	FILE *tmp = fopen("__index__.txt", "r+");
	if (tmp == NULL) return;
	File["__index__"] = tmp;
	auto in = File.find("__index__");
	fseek(in->second, 0, SEEK_SET);
	int number;
	if (fscanf(in->second, "%d", &number) == EOF) return;
	IndexID1.clear(); IndexID2.clear();
	while (number--) {
		char a[256], b[256], c[256];
		fscanf(in->second, "%s %s %s", &a, &b, &c);
		IndexID1[make_pair(a, b)] = c;
		IndexID2[c] = make_pair(a, b);
		LoadTree(c);
	}
}

void SaveIndex() {
	auto out = CreateFile("__index__", 0);
	fprintf(out->second, "%d\n", IndexID1.size());
	for (auto t : IndexID1)
		fprintf(out->second, "%s %s %s\n", t.first.first.c_str(), t.first.second.c_str(), t.second.c_str());
	fclose(out->second);
}

void LoadTree(string name) {
	if (rt.find(name) != rt.end()) return;
	auto in = File.find(name);
	if (in == File.end()) {
		File[name] = fopen((name + ".txt").c_str(), "r+");
		in = File.find(name);
	}
	fseek(in->second, 0, SEEK_SET);
	
	int number = 0; fscanf(in->second, "%d", &number);
	static map<int, BPlusTree *>ID; ID.clear();
	static map<int, int>cnt; cnt.clear();

	while (number--) {
		BPlusTree *cur = new BPlusTree();
		int fatherid;
		fscanf(in->second, "%d%d%d%d%d", &cur->id, &cur->num, &fatherid, &cur->isroot, &cur->isleaf);
		
		ID[cur->id] = cur; cnt[cur->id] = 0;
		if (fatherid > 0) {
			ID[fatherid]->son[cnt[fatherid]++] = cur;
			cur->fa = ID[fatherid];
		}
		else rt[name] = cur;
		for (int i = 0; i < cur->num; i++)
			fscanf(in->second, "%d%s", &cur->key[i].tp, cur->key[i].d);
		if (cur->isleaf) {
			for (int i = 0; i < cur->num; i++)
				fscanf(in->second, "%d", &cur->pos[i]);
		}
		
	}
	for (auto t : ID) {
		BPlusTree *cur = t.second;
		if (cur->isleaf) continue;
		assert(cur->son[cur->num] != nullptr);
		for (int i = 0; i < cur->num; i++)
			cur->son[i]->Right = cur->son[i + 1];
		for (int i = cur->num; i > 0; i--)
			cur->son[i]->Left =  cur->son[i - 1];
	}
}
void SaveNode(map<string, FILE*>::iterator out, BPlusTree *cur) {
	fprintf(out->second, "%d %d %d %d %d ", cur->id, cur->num, cur->fa == nullptr ? 0 :cur->fa->id, cur->isroot, cur->isleaf);
	for (int i = 0; i < cur->num; i++) fprintf(out->second, "%d %s ", cur->key[i].tp, cur->key[i].d);
	if (cur->isleaf) {
		for (int i = 0; i < cur->num; i++)
			fprintf(out->second, "%d ", cur->pos[i]);
	}
	else {
		for (int i = 0; i <= cur->num; i++)
			SaveNode(out, cur->son[i]);
	}
}
void SaveTree(string name) {
	auto out = CreateFile(name, 0);
	fprintf(out->second, "%d\n", CalcSize(rt[name]));
	SaveNode(out, rt[name]);
}
void SaveAllTree() {
	for (auto name : rt)
		SaveTree(name.first);
}
void Free(BPlusTree *cur) {
	if (!cur->isleaf) {
		for (int i = 0; i <= cur->num; i++)
			Free(cur->son[i]);
	}
	delete cur;
}

int CalcSize(BPlusTree *cur) {
	if (cur == nullptr) return 0;
	if (cur->isleaf) return 1;
	int ret = 1;
	for (int i = 0; i <= cur->num; i++)
		ret += CalcSize(cur->son[i]);
	return ret;
}
int FindLeaf(BPlusTree *cur, Element key) {
	while (!cur->isleaf) {
		int i;
		for (i = 0; i < cur->num; i++)
			if (key < cur->key[i]) break;
		cur = cur->son[i];
	}
	for (int i = 0; i < cur->num; i++)
		if (cur->key[i] == key) return cur->pos[i];
	return -1;
}

int Position(BPlusTree *cur, Element key) {
	for (int k = 0; k < cur->num; k++)
		if (cur->key[k] == key) return k;
	return -1;
}

int GetID(BPlusTree *cur, BPlusTree *son) {
	for (int i = 0; i < cur->num; i++)
		if (cur->son[i] == son) return i;
	return cur->num;
}

void SimpleDelete(BPlusTree *cur, int k) {
	for (int i = k >= 0 ? k : 0; i + 1 < cur->num; i++)
		cur->key[i] = cur->key[i + 1];
	if (cur->isleaf) {
		for (int i = k; i + 1 < cur->num; i++)
			cur->pos[i] = cur->pos[i + 1];
	}
	else {
		for (int i = k + 1; i < cur->num; i++)
			cur->son[i] = cur->son[i + 1];
	}
	cur->num--;
}
void SimpleInsert(BPlusTree *cur, int k, Element key, BPlusTree *son = nullptr, int offset = -1) {
	for (int i = cur->num; i > k && i > 0; i--)
		cur->key[i] = cur->key[i - 1];
	cur->key[k >= 0 ? k : 0] = key;
	if (cur->isleaf) {
		for (int i = cur->num; i > k; i--)
			cur->pos[i] = cur->pos[i - 1];
		cur->pos[k] = offset;
	}
	else {
		for (int i = cur->num + 1; i > k + 1; i--)
			cur->son[i] = cur->son[i - 1];
		cur->son[k + 1] = son;
	}
	cur->num++;
}

void Split(BPlusTree *cur) {
	BPlusTree *add = new BPlusTree();
	add->isleaf = cur->isleaf;
	add->fa = cur->fa;
	Element mid;
	if (cur->Right != nullptr)
		cur->Right->Left = add, add->Right = cur->Right;
	cur->Right = add; add->Left = cur;

	if (cur->isleaf) {
		mid = cur->key[(M + 1) / 2];
		add->num = M / 2;
		cur->num = M - add->num;
		for (int i = 0; i < add->num; i++)
			add->key[i] = cur->key[i + cur->num], add->pos[i] = cur->pos[i + cur->num];
	}
	else {
		mid = cur->key[M / 2];
		add->num = (M - 1) / 2;
		cur->num = M - 1 - add->num;
		for (int i = 0; i < add->num; i++) {
			add->key[i] = cur->key[i + cur->num + 1];
			add->son[i] = cur->son[i + cur->num + 1];
			add->son[i]->fa = add;
		}
		add->son[add->num] = cur->son[M];
		add->son[add->num]->fa = add;
	}

	if (cur->isroot) {
		cur->isroot = add->isroot = 0;
		BPlusTree *root = new BPlusTree();
		root->isroot = 1;
		root->isleaf = 0;
		root->num = 1;
		root->key[0] = mid;
		root->son[0] = cur;
		root->son[1] = add;
		cur->fa = add->fa = root;
		rt[rtIndexName] = root;
	}
	else {
		InsertNode(cur->fa, mid, add);
	}
}

int InsertLeaf(BPlusTree *cur, Element key, int offset) {
	while (!cur->isleaf) {
		int i = 0;
		for (i = 0; i < cur->num; i++)
			if (key < cur->key[i]) break;
		cur = cur->son[i];
	}
	int k = 0;
	while (k < cur->num && cur->key[k] < key) ++k;
	if (k < cur->num && cur->key[k] == key) return 0;
	SimpleInsert(cur, k, key, nullptr, offset);
	//printf("Now inserting %d %d ...\n", cur->id, cur->num);
	if (cur->num == M)
		Split(cur);
	return 1;
}
void InsertNode(BPlusTree *cur, Element key, BPlusTree *son) {
	int k = 0;
	while (k < cur->num && cur->key[k] < key) ++k;
	SimpleInsert(cur, k, key, son);
	if (cur->num == M)
		Split(cur);
}

void DelLink(BPlusTree *B) {
	BPlusTree *A = B->Left;
	BPlusTree *C = B->Right;
	if (C != nullptr)
		C->Left = A;
	if (A != nullptr)
		A->Right = C;
	delete B;
}

void DeleteNode(BPlusTree *cur, int k) {
	SimpleDelete(cur, k);
	int least = (M - 1) / 2;
	if (cur->num >= least) return;
	if (cur->isroot) {
		if (cur->num == 0 && cur->son[0] != nullptr) {
			cur->son[0]->isroot = 1;
			cur->son[0]->fa = nullptr;
			rt[rtIndexName] = cur->son[0];
			delete cur;
		}
		return;
	}
	if (cur->fa->son[0] == cur) {
		int id = GetID(cur->fa, cur);
		if (cur->Right->num > least) {
			SimpleInsert(cur, cur->num, cur->fa->key[id], cur->Right->son[0]);
			cur->Right->son[0]->fa = cur;
			cur->fa->key[id] = cur->Right->key[0];
			SimpleDelete(cur->Right, -1);
		}
		else {
			SimpleInsert(cur, cur->num, cur->fa->key[id], cur->Right->son[0]);
			cur->Right->son[0]->fa = cur;
			for (int i = 0; i < cur->Right->num; i++) {
				SimpleInsert(cur, cur->num, cur->Right->key[i], cur->Right->son[i + 1]);
				cur->Right->son[i + 1]->fa = cur;
			}
			DeleteNode(cur->fa, id);
			DelLink(cur->Right);
		}
	}
	else {
		int id = GetID(cur->fa, cur->Left);
		if (cur->Left->num > least) {
			SimpleInsert(cur, -1, cur->fa->key[id], cur->Left->son[cur->Left->num]);
			cur->Left->son[cur->Left->num]->fa = cur;
			cur->fa->key[id] = cur->Left->key[cur->Left->num - 1];
			SimpleDelete(cur->Left, cur->Left->num - 1);
		}
		else {
			SimpleInsert(cur->Left, cur->Left->num, cur->fa->key[id], cur->son[0]);
			cur->son[0]->fa = cur->Left;
			for (int i = 0; i < cur->num; i++) {
				SimpleInsert(cur->Left, cur->Left->num, cur->key[i], cur->son[i + 1]);
				cur->son[i + 1]->fa = cur->Left;
			}
			DeleteNode(cur->fa, id);
			DelLink(cur);
		}
	}
}

void DeleteLeaf(BPlusTree *cur, Element key) {
	while (!cur->isleaf) {
		int i = 0;
		for (i = 0; i < cur->num; i++)
			if (key < cur->key[i]) break;
		cur = cur->son[i];
	}
	int k = 0;
	while (k < cur->num && cur->key[k] < key) ++k;
	if (k == cur->num || cur->key[k] != key) return;
	SimpleDelete(cur, k);
	int least = (M - 1) / 2;
	if (cur->isroot || cur->num >= least) return;
	if (cur->fa->son[0] == cur) {
		int id = GetID(cur->fa, cur);
		if (cur->Right->num > least) {
			SimpleInsert(cur, cur->num, cur->Right->key[0], nullptr, cur->Right->pos[0]);
			SimpleDelete(cur->Right, 0);
			cur->fa->key[id] = cur->Right->key[0];
		}
		else {
			for (int i = 0; i < cur->Right->num; i++)
				SimpleInsert(cur, cur->num, cur->Right->key[i], nullptr, cur->Right->pos[i]);
			DeleteNode(cur->fa, id);
			DelLink(cur->Right);
		}
	}
	else {
		int id = GetID(cur->fa, cur->Left);
		if (cur->Left->num > least) {
			SimpleInsert(cur, 0, cur->Left->key[cur->Left->num - 1], nullptr, cur->Left->pos[cur->Left->num - 1]);
			SimpleDelete(cur->Left, cur->Left->num - 1);
			cur->fa->key[id] = cur->key[0];
		}
		else {
			for (int i = 0; i < cur->num; i++)
				SimpleInsert(cur->Left, cur->Left->num, cur->key[i], nullptr, cur->pos[i]);
			DeleteNode(cur->fa, id);
			DelLink(cur);
		}
	}
}

void PrintNode(BPlusTree *cur) {
	printf("%d: %d (root:%d, leaf:%d)\n", cur->id, cur->num, cur->isroot, cur->isleaf);
	if (cur->isleaf) {
		for (int i = 0; i < cur->num; i++)
			printf("%s:%d  ", cur->key[i].d, cur->pos[i]);
	}
	else {
		printf("%d ", cur->son[0]->id);
		for (int i = 0; i < cur->num; i++)
			printf("(%s) %d ", cur->key[i].d, cur->son[i + 1]->id);
	}
	puts(""); puts("");
}
void PrintTree(BPlusTree *cur) {
	PrintNode(cur);
	if (cur->isleaf) return;
	for (int i = 0; i <= cur->num; i++)
		PrintTree(cur->son[i]);
}