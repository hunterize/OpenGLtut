#include "IOManager.h"

#include <fstream>

bool IOManager::ReadFiletoBuffer(const std::string& filePath, std::vector<unsigned char>& buffer)
{
	std::ifstream file(filePath, std::ios::binary);
	if (file.fail())
	{
		perror(filePath.c_str());
		return false;
	}

	int fileSize;

	std::streampos pos;
	

	//int fileSize;


	file.seekg(0, std::ios::end);
	fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	//reduce the filesize by header bytes
	fileSize -= file.tellg();

	buffer.resize(fileSize);

	file.read((char*)&buffer[0], fileSize);

	file.close();

	return true;
}