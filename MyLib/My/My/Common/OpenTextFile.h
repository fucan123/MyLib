#pragma once
#include <iostream>

class OpenTextFile
{
public:
	/* 打开一个文件 */
	bool Open(const char* file);

	/* 关闭它 */
	void Close();

	/* 获取一行数据 返回: 0=注释行 -1=结束 */
	int  GetLine(char* data, int maxCount);

	/* 获取所有行数, 不包括注释 */
	int  GetCount();

	/* 重置 */
	void ReWind();
protected:
	FILE* m_File;
	int   m_Count = 0;
	bool  m_IsGetCount = false;
};