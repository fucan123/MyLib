#include "MString.h"
#include <string.h>
#include <stdio.h>

MString::MString()
{
	_Init();
}

MString::MString(int value)
{
	_Init();

	char _str[16];
	sprintf_s(_str, "%d", value);
	_Copy(_str);
}

MString::MString(const char* _str)
{
	_Init();
	_Copy(_str);
}

MString::MString(MString& obj)
{
	_Init();
	_Copy(obj.GetBuffer());
}

MString::~MString()
{
	_Destory();
}

void MString::operator=(const char* _str)
{
	_Init();
	_Copy(_str);
}

void MString::operator=(MString& obj)
{
	_Init();
	_Copy(obj.GetBuffer());
}

void MString::operator+=(int value)
{
	char _str[16];
	sprintf_s(_str, "%d", value);
	_Cat(_str);
}

void MString::operator+=(const char* _str)
{
	_Cat(_str);
}

void MString::operator+=(MString& obj)
{
	_Cat(obj.GetBuffer());
}

bool MString::operator==(const char* _str)
{
	return strcmp(m_pStr, _str) == 0;
}

bool MString::operator==(MString& obj)
{
	return strcmp(m_pStr, obj.GetBuffer()) == 0;
}

MString& MString::operator+(int value)
{
	char _str[16];
	sprintf_s(_str, "%d", value);
	_Cat(_str);

	return *this;
}

MString& MString::operator+(const char* _str)
{
	_Cat(_str);
	return *this;
}

MString& MString::operator+(MString& obj)
{
	_Cat(obj.GetBuffer());
	return *this;
}

const char* MString::GetBuffer()
{
	return m_pStr;
}

__int64 MString::GetLength()
{
	return m_nLen;
}

void MString::_Init()
{
	m_chStr[0] = 0;
	m_pStr = m_chStr;
	m_nAllocLen = sizeof(m_chStr);
	m_nLen = 0;
}

void MString::_Copy(const char* _source)
{
	size_t len = strlen(_source);
	if (len >= m_nAllocLen) {
		m_nAllocLen = len * 3 + 1;
		m_pStr = new char[m_nAllocLen];
	}
	strcpy(m_pStr, _source);
	m_nLen = len;
}

void MString::_Cat(const char* _source)
{
	m_nLen += strlen(_source);
	if (m_nLen >= m_nAllocLen) {
		m_nAllocLen = m_nLen * 3 + 1;
		char* str = new char[m_nAllocLen];
		strcpy(str, m_pStr);

		_Destory();
		m_pStr = str;
	}
	strcat(m_pStr, _source);
}

void MString::_Destory()
{
	if (m_pStr && m_pStr != m_chStr) {
		delete[] m_pStr;
		m_pStr = nullptr;
	}	
}
