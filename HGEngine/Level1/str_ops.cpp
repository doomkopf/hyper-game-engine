#include "str_ops.h"
#include <string.h>

void GetKey(const char *cmd, char *key)
{
	for(int i = 0; cmd[i] != ' ' && cmd[i] != 0; i++)
		key[i] = cmd[i];

	key[i] = 0;
}

bool GetValueINT(const char *cmd, int *integer)
{
	int len = (int)strlen(cmd);
	int ind, mul;

	for(int i = 0; ; i++)
	{
		if(cmd[i] == ' ')
		{
			if(cmd[++i] == 0)
				return false;
			break;
		}
		if(cmd[i] == 0)
			return false; // error: command-string terminates in key part (key has no value)
	}

	ind = i;
	mul = 1;

	for(i = 1; i < len - ind; i++)
		mul *= 10;

	*integer = 0;
	for(; cmd[ind] != 0; ind++, mul /= 10)
	{
		if(cmd[ind] < '0' || cmd[ind] > '9')
			return false;

		*integer += (cmd[ind] - 48) * mul;
	}

	return true;
}