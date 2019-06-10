#include "BasicManager.h"
#include <map>

map<pair<string, string>, string>IndexID1;
map<string, pair<string, string>>IndexID2;
map<string, FILE*>File;

map<string, FILE*>::iterator CreateFile(string name, int binary) {
	auto it = File.find(name);
	if (it != File.end()) {
		fclose(it->second);
		File.erase(it);
	}
	//printf("%s\n", name.c_str());
	FILE *tmp =  fopen((name + ".txt").c_str(), binary ? "wb"  : "w" ); fclose(tmp);
	string t = binary ? "rb+" : "r+";
	File[name] = fopen((name + ".txt").c_str(), t.c_str());
	return File.find(name);
}

void DeleteFile(string name) {
	auto it = File.find(name);
	if (it != File.end()) {
		fclose(it->second);
		File.erase(it);
		remove((name + ".txt").c_str());
	}
}