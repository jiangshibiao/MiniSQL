//From zimpha

#ifndef _BUFFERMANAGER_H
#define _BUFFERMANAGER_H

#include <list>
#include <string>
#include <cstdio>
#include <map>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include "BasicManager.h"

/**
	该文件是MiniSQL中的缓冲区管理模块头文件.
	推荐采取的文件存储方式 :
		为每一张表建立一个文件.
		为每一个索引建立一个文件
	通过传入文件名到该头文件中定义的函数中的方式来读取某个表/索引中的特定块.
	注意, 读取到的块的开头地址(offset)都是4096的整数倍, 每个块的大小为4KB
**/

class BFM;

struct Block {
	public:
		char data[BLOCKSIZE];
		string fileName;
		int offset, num;
		Block(string _fileName, int _offset, int _num) {
			num = _num;
			memset(data, 0, BLOCKSIZE);
			offset = _offset;
			fileName = _fileName;
		}
	private:
		friend BFM;
};

typedef std::list<Block>::iterator bufferIter;
typedef std::pair <std::string, int> tag;
typedef std::map <tag, bufferIter>::iterator tableIter;

class BFM {
public:
	static bufferIter InsertBlock(Block tmp);
	static bufferIter BufferManagerRead(const std::string &fileName, int offset, int num);
	static void BufferManagerWrite(const Block &b);
	static void BufferManagerFlush();
	static void deleteFile(const std::string &fileName);
	static Element LoadData(bufferIter it, int pos);
	static void SetData(bufferIter it, int pos, Element val);
	static void SetData(Block *now, int pos, Element val);

private:
	static std::list <Block> buffer;
	static std::map <tag, bufferIter> table;
};

#endif