#include "RecordManager.h"
extern int memory;

int Record::FindRecord(string TableName, vector<InputData>Table, vector<Condition>Cond, int visual = 0, int ins = 1) {
	//若 visual = 1，打印表格
	//找到至少一个返回1，没找到返回0，类型出错返回-1

	vector<int>Index;
	Index.resize(Cond.size());
	for (int i = 0; i < Cond.size(); i++) {
		Index[i] = -1;
		for (int k = 0; k < Table.size(); k++)
			if (Table[k].name == Cond[i].name)
				Index[i] = k;
		if (Index[i] == -1) return -1;
	}

	vector<pair<string, int> >edit;
	for (int i = 0; i < Table.size(); i++) {
		auto it = IndexID1.find(make_pair(TableName, (string)Table[i].name));
		if (it != IndexID1.end())
			edit.push_back(make_pair(it->second, i));
	}

	int appear = 0;
	
	auto in = File.find(TableName);
	if (in == File.end()) return 0;
	fseek(in->second, 0, SEEK_END);
	int where = ftell(in->second);
	int blocksize = DATASIZE * Table.size();
	int offset = where / blocksize;

	for (int i = 0; i < offset; i++) {
		bufferIter it = BFM::BufferManagerRead(TableName , i * blocksize, Table.size());
		vector<Element>rec;
		for (int j = 0; j < Table.size(); j++) {
			Element cur = BFM::LoadData(it, j * DATASIZE);
			if (cur.tp != NONE) rec.push_back(cur);
		}
		if (rec.size() > 0) {
			//printf("%d %d\n", rec.size(), Table.size());
			assert(rec.size() == Table.size());
			int ok = 1;
			for (int t = 0; t < Cond.size(); t++){
				Element ret = rec[Index[t]], k = Cond[t].p;
				switch (Cond[t].op) {
					case LESS: ok &= ret < k; break;
					case MORE: ok &= ret > k; break;
					case LESSQ: ok &= ret <= k; break;
					case MOREQ: ok &= ret >= k; break;
					case EQUAL: ok &= ret == k; break;
					case NOTEQUAL: ok &= ret != k; break;
					default: assert(0);
				}
			}
			if (ok) {
				if (visual) {
					if (!appear) {
						printf("The result is:\n");
						for (auto k : Table)
							printf("%12s", k.name);
						printf("\n");
						
					}
					for (auto k : rec)
						printf("%12s", k.d);
					printf("\n");
				}
				if (ins) {
					for (auto k : edit) 
						InsertLeaf(rt[rtIndexName = k.first], rec[k.second], i * blocksize);
					//PrintTree(rt[rtIndexName]);
				}
				appear = 1;
			}
			rec.clear();
		}
	}
	if (visual) printf("\n");
	return appear;
}

int Record::Delete(string TableName, vector<InputData>Table, vector<Condition>Cond) {
	vector<int>Index;
	Index.resize(Cond.size());
	for (int i = 0; i < Cond.size(); i++) {
		Index[i] = -1;
		for (int k = 0; k < Table.size(); k++)
			if (Table[k].name == Cond[i].name)
				Index[i] = k;
		if (Index[i] == -1) return -1;
	}

	vector<pair<string, int> >edit;
	for (int i = 0; i < Table.size(); i++) {
		auto it = IndexID1.find(make_pair(TableName, (string)Table[i].name));
		if (it != IndexID1.end())
			edit.push_back(make_pair(it->second, i));
	}

	int delnum = 0;

	auto in = File.find(TableName);
	if (in == File.end()) return 0;
	fseek(in->second, 0, SEEK_END);
	int where = ftell(in->second);
	int blocksize = DATASIZE * Table.size();
	int offset = where / blocksize;

	for (int i = 0; i < offset; i++) {
		bufferIter it = BFM::BufferManagerRead(TableName, i * blocksize, Table.size());
		vector<Element>rec;
		for (int j = 0; j < Table.size(); j++) {
			Element cur = BFM::LoadData(it, j * DATASIZE);
			if (cur.tp == NONE) break; rec.push_back(cur);
		}
		if (rec.size()) {
			assert(rec.size() == Table.size());
			int ok = 1;
			for (int t = 0; t < Cond.size(); t++) {
				Element ret = rec[Index[t]], k = Cond[t].p;
				switch (Cond[t].op) {
					case LESS: ok &= ret < k; break;
					case MORE: ok &= ret > k; break;
					case LESSQ: ok &= ret <= k; break;
					case MOREQ: ok &= ret >= k; break;
					case EQUAL: ok &= ret == k; break;
					case NOTEQUAL: ok &= ret != k; break;
					default: assert(0);
				}
			}
			if (ok) {
				delnum++;
				for (auto t : edit)
					DeleteLeaf(rt[rtIndexName = t.first], rec[t.second]);
				for (int t = 0; t < rec.size(); t++) 
					rec[t].tp = 0, BFM::SetData(it, t * DATASIZE, rec[t]);
			}
			rec.clear();
		}
	}
	return delnum;
}


int Record::Insert(string TableName, vector<Element>Data) {
	try {
		auto out = File.find(TableName);
		fseek(out->second, 0, SEEK_END);
		int offset = ftell(out->second);

		Block *tmp = new Block(TableName, offset, Data.size());
		for (int i = 0; i < Data.size(); i++)
			BFM::SetData(tmp, i * DATASIZE, Data[i]);
		bufferIter it = BFM::InsertBlock(*tmp);
		BFM::BufferManagerWrite(*tmp);
		return offset;
	}
	catch (...) {
		printf("The impossible thing happens in buffer insert.\n");
	}
}