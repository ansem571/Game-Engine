#pragma once
#include <string>
#include <fstream>
#include <Qt\qdebug.h>
class FileIO
{
public:
	static std::string file2String(const char* path)
	{ 
		std::ifstream meInput(path);
	if(!meInput.good())
	{
		qDebug() << path << " file failed to load...";
		exit(1);
	}

	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>());
	meInput.close();
	}
	
};

