#pragma once

#define EXPLODE_MAX_LENGTH 10
#define EXPLODE_MAX_BYTE   128

class Explode
{
public:
	Explode(const char  separator, const char* string);
	Explode(const char* separator, const char* string);
	~Explode();
	char* GetValue(int index);
	int   GetValue2Int(int index, int default_value=0);
	char* operator[](int index);
	int GetCount();
protected:
	char        m_SeparatorCh[2];
	const char* m_Separator;
	const char* m_String;
	char        m_Value[EXPLODE_MAX_LENGTH][EXPLODE_MAX_BYTE];
	int         m_Count;
protected:
	void Parse();
	int  Start();
	void Copy(int index, const char* start, const char* end);
};