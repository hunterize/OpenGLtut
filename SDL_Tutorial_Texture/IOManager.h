#pragma once


#include <vector>

class IOManager
{
public:
	static bool ReadFiletoBuffer(const std::string& filePath, std::vector<unsigned char>& buffer);
};

