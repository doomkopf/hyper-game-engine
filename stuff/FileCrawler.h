#ifndef FILECRAWLER_H_
#define FILECRAWLER_H_

#include "small_windows.h"

class CFileCrawler
{
public:
	CFileCrawler(const char *);
	~CFileCrawler();
	bool NextFile(char *);

protected:
	HANDLE hfind;
	WIN32_FIND_DATA fd;
};

#endif