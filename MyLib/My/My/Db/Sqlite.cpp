#include "Sqlite.h"
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

// ...
Sqlite::Sqlite(const char * db_name)
{
	Open(db_name);
}

// 打开数据库
bool Sqlite::Open(const char * db_name)
{
	if (!db_name)
		return false;

	//printf("数据库文件:%s\n", db_name);
	return sqlite3_open(db_name, &m_db) == SQLITE_OK;
}

// 数据表是否存在
bool Sqlite::TableIsExists(const char* table)
{
	char sql[128];
	sprintf_s(sql, "SELECT name FROM sqlite_master WHERE name='%s' ORDER BY name", table);

	int ncolumn = 0;   //查询到数据的列数
	char **azResult = NULL;   //二维数组存放结果
	int row = SelectAll(sql, &azResult, &ncolumn);
	FreeResult(azResult);

	return row > 0;
}

// 获取条数
int Sqlite::GetRowCount(const char * sql)
{
	int ncolumn = 0;   //查询到数据的列数
	char **azResult = NULL;   //二维数组存放结果
	int row = SelectAll(sql, &azResult, &ncolumn);
	FreeResult(azResult);

	return row;
}

// 获取单字段数值
bool Sqlite::GetOneCol(const char * sql, char dst[])
{
	int ncolumn = 0;   //查询到数据的列数
	char **azResult = NULL;   //二维数组存放结果
	int row = SelectAll(sql, &azResult, &ncolumn);
	if (ncolumn == 0 || row == 0)
		return false;

	strcpy(dst, azResult[ncolumn]);
	FreeResult(azResult);

	return true;
}

// 查询全部[返回条数]
int Sqlite::SelectAll(const char* sql, char *** result, int* col, char** msg)
{
	int nrow = 0;       //查询到数据的行数
	if (sqlite3_get_table(m_db, sql, result, &nrow, col, msg) != SQLITE_OK) { //查询数据库
		printf("SQL错误: %s\n", sqlite3_errmsg(m_db));
	}; 
	return nrow;
}

// 释放查询结果
void Sqlite::FreeResult(char ** result)
{
	sqlite3_free_table(result);
}

// 执行SQL
bool Sqlite::Exec(const char* sql)
{
	if (sqlite3_exec(m_db, sql, nullptr, nullptr, nullptr) != SQLITE_OK) {
		printf("SQL错误: %s\n", sqlite3_errmsg(m_db));
		return false;
	}
	return true;
}

// 关闭
void Sqlite::Close()
{
	sqlite3_close(m_db);
}

// 数目
int Sqlite::Count()
{
	strcpy(m_sField, "COUNT(*)");
	return GetCol2Int();
}

// 获取一行信息
char** Sqlite::Get()
{
	MakeSelectSql();

	int row, col;
	return List(row, col);
}

// 获取单个字段信息
char* Sqlite::GetCol(char* dst)
{
	char** result = Get();
	if (!result) {
		if (dst) {
			dst[0] = 0;
		}
		return nullptr;
	}

	if (dst) {
		strcpy(dst, result[0]);
	}
	return result[0];
}

// 获取单个字段信息
int Sqlite::GetCol2Int(int dv)
{
	char** result = Get();
	int v = result ? atoi(result[0]) : dv;
	FreeResult();
	return v;
}

// 获取列表
char** Sqlite::List(int& row, int& col)
{
	MakeSelectSql();

	row = SelectAll(GetSql(), &m_sResult, &col);
	if (col == 0 || row == 0)
		return nullptr;

	return &m_sResult[col];
}

// 释放资源
void Sqlite::FreeResult()
{
	if (m_sResult) {
		FreeResult(m_sResult);
		m_sResult = nullptr;
	}
}
