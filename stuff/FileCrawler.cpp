#include "FileCrawler.h"

CFileCrawler::CFileCrawler(const char *path)
{
	hfind = FindFirstFile(path, &fd);
}

CFileCrawler::~CFileCrawler()
{
	FindClose(hfind);
}

bool CFileCrawler::NextFile(char *file)
{
	do
	{
		if(!FindNextFile(hfind, &fd))
			return false;
	}
	while(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

	strcpy(file, fd.cFileName);

	return true;
}