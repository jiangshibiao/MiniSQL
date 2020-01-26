//For reference
#include "bufferManager.h"
#include <iostream>

using namespace std;

map <tag, bufferIter> BFM::table;
list <Block> BFM::buffer;

bufferIter BFM::InsertBlock(Block tmp) {
	buffer.push_back(tmp);
	bufferIter B = --buffer.end();
	tag T = make_pair(tmp.fileName, tmp.offset);
	table.insert(make_pair(T, B));

	if (buffer.size() > BUFFERSIZE) {
		bufferIter del = buffer.begin();
		BufferManagerWrite(*del);
		table.erase(make_pair(del->fileName, del->offset));
		buffer.erase(del);
	}
	return B;
}

bufferIter BFM::BufferManagerRead(const string &fileName, int offset, int num)
{
	auto it = table.find(make_pair(fileName, offset));
	if (it != table.end()) {
		Block tmp = *(it->second);
		buffer.erase(it->second);  
		buffer.push_back(tmp);

		bufferIter B = --buffer.end();
		tag T = make_pair(tmp.fileName, tmp.offset);
		return table.find(T)->second = B;
	}
	else {
		auto in = File.find(fileName);
		try {
			fseek(in->second, offset, SEEK_SET);
			Block tmp = Block(fileName, offset, num);
			fread(&tmp.data, num * DATASIZE, 1, in->second);
			return InsertBlock(tmp);
		}
		catch (...){
			printf("The impossible thing happens in buffer read.\n");
		}
	}
}

void BFM::BufferManagerWrite(const Block &b)
{
	try {
		auto in = File.find(b.fileName);
		assert(in->second != nullptr);
		fseek(in->second, b.offset, SEEK_SET);
		fwrite(&b.data, b.num * DATASIZE, 1, in->second);
	}
	catch (...) {
		printf("The impossible thing happens in buffer write.\n");
	}
}


void BFM::BufferManagerFlush()
{
	for (bufferIter T = buffer.begin(); T != buffer.end();) {
		BufferManagerWrite(*T++);
	}
	buffer.clear();
	table.clear();
}

void BFM::deleteFile(const string &fileName)
{
	while (true){
		auto it = table.lower_bound(make_pair(fileName, 0));
		if (it == table.end() || (it->first).first != fileName) break;
		BufferManagerWrite(*(it->second));
		buffer.erase(it->second);
		table.erase(it);
	}
}

Element BFM::LoadData(bufferIter it, int pos) {
	Element ret;
	ret.tp = (*it).data[pos];
	for (int i = 0; i < 255; i++)
		ret.d[i] = (*it).data[pos + i + 1];
	return ret;
}

void BFM::SetData(Block *now, int pos, Element val) {
	now->data[pos] = val.tp;
	for (int i = 0; i < 255; i++)
		now->data[pos + i + 1] = val.d[i];
}

void BFM::SetData(bufferIter it, int pos, Element val) {
	it->data[pos] = val.tp;
	for (int i = 0; i < 255; i++)
		it->data[pos + i + 1] = val.d[i];
}



