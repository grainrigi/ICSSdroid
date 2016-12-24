#include "pch.h"
#include "util/file/BinaryFile.h"

using ICSS::file::BinaryFile;

using ICSS::container::Uint8Array;

BinaryFile BinaryFile::loadFromFile(const std::string & fileName)
{
	std::ifstream fs(fileName, std::ios::in | std::ios::binary);
	BinaryFile bf;

	//Get the endpos
	fs.seekg(0, std::fstream::end);
	uint32_t endPos = fs.tellg();
	fs.clear();

	//Get the start pos
	fs.seekg(0, std::fstream::beg);
	uint32_t begPos = fs.tellg();

	//Get the filesize
	uint32_t size = endPos - begPos;

	bf.m_data = Uint8Array(size);
	
	fs.read(reinterpret_cast<char*>(bf.m_data.getPtr()), size);
	fs.close();

	return bf;
}

BinaryFile::BinaryFile(void)
{
}
