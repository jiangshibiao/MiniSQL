#include "CatalogManager.h"

void Catalog::ReadTable() {
	FILE *tmp = fopen("__table__.txt", "r+");
	if (tmp == NULL) return;
	File["__table__"] = tmp;
	auto in = File.find("__table__");
	fseek(in->second, 0, SEEK_SET);

	char curTableName[252];
	while (fscanf(in->second, "%s", curTableName) != EOF) {
		int number, valid;
		fscanf(in->second, "%d %d", &valid, &number);
		while (number--) {
			char name[252]; int tp, uni, key, len;
			fscanf(in->second, "%s %d %d %d %d", name, &tp, &key, &uni, &len);
		}
		if (valid) File[curTableName] = fopen(((string)curTableName + ".txt").c_str(), "r+");
	}
}

vector<InputData> Catalog::GetTable(string TableName) {
	auto in = File.find("__table__");
	vector<InputData>ret;
	char curTableName[252];
	fseek(in->second, 0, SEEK_SET);
	while (fscanf(in->second, "%s", curTableName) != EOF) {
		int number, valid;
		if (fscanf(in->second, "%d %d", &valid, &number) == EOF) return ret;
		int same = (string)curTableName == TableName && valid;
		for (int id = 0; id < number; id++){
			char name[252]; int tp, uni, key, len;
			if (fscanf(in->second, "%s %d %d %d %d", name, &tp, &key, &uni, &len) == -1) return ret;
			InputData tmp = InputData(name, tp, key, uni, len);
			if (same) ret.push_back(tmp);
		}
		if (same) return ret;
	}
	return ret;
}

int Catalog::ExistTable(string TableName) {
	return File.find(TableName) != File.end();
}

void Catalog::CreateTable(string TableName, vector<InputData>Table) {
	auto out = File.find("__table__");
	if (out == File.end()) out = CreateFile("__table__", 0);
	fseek(out->second, 0, SEEK_END);
	fprintf(out->second, "%s 1 %d", TableName.c_str(), Table.size());
	//printf("%s 1 %d", TableName.c_str(), Table.size());
	for (auto t : Table)
		fprintf(out->second, " %s %d %d %d %d", t.name, (int)t.tp, t.key, t.uni, t.len);
	fprintf(out->second, " ");
	//printf("ok\n");
}

int Catalog::DropTable(string TableName) {
	auto out = File.find("__table__");
	fseek(out->second, 0, SEEK_SET);
	char curTableName[252];
	while (fscanf(out->second, "%s", curTableName) != EOF) {
		int number, valid, loc = ftell(out->second) + 1;
		if (fscanf(out->second, "%d %d", &valid, &number) == EOF) return 0;
		
		int same = (string)curTableName == TableName && valid;
		while (number--) {
			char name[252]; int tp, uni, key, ind, len;
			if (fscanf(out->second, "%s %d %d %d %d", name, &tp, &uni, &key, &len) == EOF) return 0;
		}
		if (same) {
			fseek(out->second, loc, SEEK_SET);
			fprintf(out->second, "0");
			return 1;
		}
	}
	
	return 0;
}