#pragma once

#define EXPLODE_MAX_LENGTH 16
#define EXPLODE_MAX_BYTE   256

class Explode
{
public:
	// separator=·Ö¸ô·û string=×Ö·û´®
	Explode(const char  separator, const char* string);
	// separator=·Ö¸ô·û string=×Ö·û´®
	Explode(const char* separator, const char* string);
	~Explode();
	char* GetValue(int index);
	int   GetValue2Int(int index, int default_value=0, bool is_trim=true);
	int   GetHexValue2Int(int index, int default_value=0, bool is_trim=true);
	int   DecToInt(const char* str, int default_value=0);
	int   HexToInt(const char* str, int default_value = 0);
	char* operator[](int index);
	int GetCount();
protected:
	char        m_SeparatorCh[2];
	const char* m_Separator;
	const char* m_String;
	char*       m_Value[EXPLODE_MAX_LENGTH];
	int         m_Count;
protected:
	void Parse();
	int  Start();
	void Copy(int index, const char* start, const char* end);
};