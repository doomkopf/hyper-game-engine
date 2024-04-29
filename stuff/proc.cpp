#include <stdio.h>
#include <string.h>
#include <math.h>

int GetPossibleType(char *str)
{
	bool dot = false;

	for(int i = 0; str[i]; i++)
	{
		if((str[i] < 48 || str[i] > 57) && str[i] != '.')
			return 0;

		if(str[i] == '.')
		{
			if(!dot)
				dot = true;
			else
				return 0;
		}
	}

	if(dot)
		return 2;

	return 1;
}

bool StrToInt(char *str, int *integer)
{
	int i, len;

	if(GetPossibleType(str) != 1)
		return false;

	*integer = 0;

	len = (int)strlen(str);

	for(i = 0; i < str[i]; i++)
		*integer += (str[i] - 48) * (int)pow(10, len - i - 1);

	return true;
}

bool StrToInt(char *str, unsigned int *integer)
{
	int i, len;

	if(GetPossibleType(str) != 1)
		return false;

	*integer = 0;

	len = (int)strlen(str);

	for(i = 0; i < str[i]; i++)
		*integer += (str[i] - 48) * (int)pow(10, len - i - 1);

	return true;
}

bool StrToInt(char *str, short *integer)
{
	int i, len;

	if(GetPossibleType(str) != 1)
		return false;

	*integer = 0;

	len = (int)strlen(str);

	for(i = 0; i < str[i]; i++)
		*integer += (str[i] - 48) * (int)pow(10, len - i - 1);

	return true;
}

bool StrToInt(char *str, unsigned short *integer)
{
	int i, len;

	if(GetPossibleType(str) != 1)
		return false;

	*integer = 0;

	len = (int)strlen(str);

	for(i = 0; i < str[i]; i++)
		*integer += (str[i] - 48) * (int)pow(10, len - i - 1);

	return true;
}

bool StrToInt(char *str, char *integer)
{
	int i, len;

	if(GetPossibleType(str) != 1)
		return false;

	*integer = 0;

	len = (int)strlen(str);

	for(i = 0; i < str[i]; i++)
		*integer += (str[i] - 48) * (int)pow(10, len - i - 1);

	return true;
}

bool StrToInt(char *str, unsigned char *integer)
{
	int i, len;

	if(GetPossibleType(str) != 1)
		return false;

	*integer = 0;

	len = (int)strlen(str);

	for(i = 0; i < str[i]; i++)
		*integer += (str[i] - 48) * (int)pow(10, len - i - 1);

	return true;
}

bool StrToFloat(char *str, float *f)
{
	int i, len2c = 0, t;
	bool beforec = true;
	double d = 0;

	if(GetPossibleType(str) == 0)
		return false;

	for(i = 0; str[i]; i++)
	{
		if(str[i] == '.')
		{
			len2c = i;
			break;
		}
	}

	for(i = t = 0; str[i]; i++)
	{
		if(str[i] == '.')
		{
			beforec = false;
			continue;
		}

		d += (str[i] - 48) * pow(10, beforec ? len2c - i - 1 : -(++t));
	}

	*f = (float)d;

	return true;
}

void IntToStr(int integer, char *str)
{
	for(int i = 1; (integer / i) > 9; i *= 10);
	for(int j = 0; i > 0; i /= 10)
		str[j++] = ((integer / i) - ((integer / (i * 10)) * 10)) + 48;
	str[j] = 0;
}

void FloatToStr(float f, char *str)
{
}

void CleanStr(char *str, int size)
{
	for(int i = 0; str[i] != 0; i++)
	{
		if(i >= size - 1)
			return;
	}

	for(i++; i < size; i++)
		str[i] = 0;
}

bool GetFileExt(char *file, char *ext)
{
	bool dot = false;
	int i, j;

	for(i = 0; file[i]; i++)
	{
		if(file[i] == '.')
		{
			dot = true;
			i++;
			break;
		}
	}

	if(!dot)
		return false;

	for(j = 0; file[i]; i++, j++)
	{
		ext[j] = file[i];
	}
	ext[j] = 0;

	return true;
}

void NormValueF(float *f, int step)
{
	bool min = false;
	int r;

	if(*f < 0)
	{
		min = true;
		*f = -*f;
	}

	r = int(*f) % step;
	*f = float((r < (step / 2)) ? int(*f) - r : (int(*f) - r) + step);

	if(min)
		*f = -*f;
}