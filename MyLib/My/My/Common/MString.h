#pragma once

class MString
{
private:
	char    m_chStr[100];
	char*   m_pStr;
	__int64 m_nAllocLen;
	__int64 m_nLen;
public:
	MString();
	MString(int value);
	MString(const char* _str);
	MString(MString& obj);
	~MString();

	void operator=(const char* _str);
	void operator=(MString& obj);
	void operator+=(int value);
	void operator+=(const char* _str);
	void operator+=(MString& obj);

	bool operator==(const char* _str);
	bool operator==(MString& obj);

	MString& operator+(int value);
	MString& operator+(const char* _str);
	MString& operator+(MString& obj);
	const char* GetBuffer();
	__int64 GetLength();
private:
	void _Init();
	void _Copy(const char* _source);
	void _Cat(const char* _source);
	void _Destory();
};