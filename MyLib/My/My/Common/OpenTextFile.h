#pragma once
#include <iostream>

class OpenTextFile
{
public:
	bool Open(const char* file);
	void Close();
	int  GetLine(char* data, int maxCount);
	int  GetCount();
	void ReWind();
protected:
	FILE* m_File;
	int   m_Count = 0;
	bool  m_IsGetCount = false;
};