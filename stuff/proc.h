int GetPossibleType(char *);
bool StrToInt(char *, int *);
bool StrToInt(char *, unsigned int *);
bool StrToInt(char *, short *);
bool StrToInt(char *, unsigned short *);
bool StrToInt(char *, char *);
bool StrToInt(char *, unsigned char *);
bool StrToFloat(char *, float *);
void IntToStr(int, char *);
void FloatToStr(float, char *);

void CleanStr(char *, int); // fills the buffer after zero termination with zeros

bool GetFileExt(char *, char *);
void NormValueF(float *, int);