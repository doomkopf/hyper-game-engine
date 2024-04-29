#ifndef FILEBUF_H_
#define FILEBUF_H_

#include <stdio.h>

enum jump_t
{
	J_START = 0,
	J_CUR = 1,
	J_END = 2
};

class CFileBuf
{
public:
	CFileBuf();
	~CFileBuf();
	bool Open(const char *, bool, bool buffering = true);
	int Read(void *, int);
	bool GetByte(unsigned char *);
	void Write(const void *, int);
	void Jump(int, jump_t);
	void Close();
	bool _EOF();
	int GetSize();

protected:
	unsigned char *data;
	int fp, size;
	bool read, buf;
	FILE *f;
};

#endif