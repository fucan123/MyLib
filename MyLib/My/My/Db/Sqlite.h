#pragma once
#include "Sql.h"
#include "sqlite3.h"

class Sqlite: public Sql
{
public:
	// ...
	Sqlite(const char* db_name=nullptr);
	// 打开数据库
	bool Open(const char* db_name = nullptr);
	// 数据表是否存在
	bool TableIsExists(const char* table);
	// 获取条数
	int  GetRowCount(const char* sql);
	// 获取单字段数值
	bool GetOneCol(const char* sql, char dst[]);
	// 查询全部[返回条数]
	int  SelectAll(const char* sql, char*** result, int* col, char** msg=nullptr);
	// 释放查询结果
	void FreeResult(char** result);
	// 执行SQL
	bool Exec(const char* sql);
	// 关闭
	void Close();
public:
	// 数目
	virtual int Count();
	// 获取一行消息
	virtual char** Get();
	// 获取单个字段信息
	virtual char* GetCol(char* dst=nullptr);
	// 获取单个字段信息
	virtual int GetCol2Int(int dv=0);
	// 获取列表
	virtual char** List(int& row, int& col);
	// 释放资源
	virtual void FreeResult();
protected:
	// 数据库句柄
	sqlite3* m_db = nullptr;
};