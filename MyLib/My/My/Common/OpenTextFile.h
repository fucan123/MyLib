#pragma once
#include <iostream>

class OpenTextFile
{
public:
	/* ��һ���ļ� */
	bool Open(const char* file);

	/* �ر��� */
	void Close();

	/* ��ȡһ������ ����: 0=ע���� -1=���� */
	int  GetLine(char* data, int maxCount);

	/* ��ȡ��������, ������ע�� */
	int  GetCount();

	/* ���� */
	void ReWind();
protected:
	FILE* m_File;
	int   m_Count = 0;
	bool  m_IsGetCount = false;
};