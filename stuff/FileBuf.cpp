#include "stdafx.h"
#include "FileBuf.h"
#include <memory.h>

CFileBuf::CFileBuf()
{
	data = NULL;
	fp = NULL;
	f = NULL;
	size = 0;
}

CFileBuf::~CFileBuf()
{
	Close();
}

void CFileBuf::Close()
{
	if(data)
	{
		delete [] data;
		data = NULL;
	}

	if(f)
	{
		fclose(f);
		f = NULL;
	}

	fp = 0;
	size = 0;
}

bool CFileBuf::Open(const char *file, bool r, bool buffering)
{
	Close();

	read = r;
	buf = buffering;

	if(file[0] == 0)
		return false;

	if(read)
	{
		f = fopen(file, "rb");
		if(!f)
			return false;

		fseek(f, 0, SEEK_END);
		size = ftell(f);
		fseek(f, 0, SEEK_SET);

		if(buf)
		{
			data = new unsigned char[size];
			fread(data, size, 1, f);
			fclose(f);
		}
	}
	else
	{
		f = fopen(file, "w+b");
		if(!f)
			return false;
	}

	return true;
}

int CFileBuf::Read(void *d, int s)
{
	if(!read || _EOF())
		return 0;

	if(buf)
	{
		int dif = size - fp;

		if(s > dif)
		{
			memcpy(d, &data[fp], dif);
			fp += dif;

			return dif;
		}

		memcpy(d, &data[fp], s);
		fp += s;

		return s;
	}

	return (int)fread(d, 1, s, f);
}

bool CFileBuf::GetByte(unsigned char *byte)
{
	if(!read || fp == size)
		return false;

	*byte = data[fp++];

	return true;
}

void CFileBuf::Write(const void *d, int s)
{
	if(read)
		return;

	fwrite(d, s, 1, f);
}

void CFileBuf::Jump(int s, jump_t j)
{
	switch(j)
	{
	case J_START:
		fp = s;
		break;

	case J_CUR:
		fp += s;
		break;

	case J_END:
		fp = (size - 1) - s;
		break;
	}
}

int CFileBuf::GetSize()
{
	return size;
}

bool CFileBuf::_EOF()
{
	if(buf)
		return (fp >= (size - 1));

	return feof(f) ? true : false;
}