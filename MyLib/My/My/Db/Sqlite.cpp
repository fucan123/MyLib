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

// �����ݿ�
bool Sqlite::Open(const char * db_name)
{
	if (!db_name)
		return false;

	//printf("���ݿ��ļ�:%s\n", db_name);
	return sqlite3_open(db_name, &m_db) == SQLITE_OK;
}

// ���ݱ��Ƿ����
bool Sqlite::TableIsExists(const char* table)
{
	char sql[128];
	sprintf_s(sql, "SELECT name FROM sqlite_master WHERE name='%s' ORDER BY name", table);

	int ncolumn = 0;   //��ѯ�����ݵ�����
	char **azResult = NULL;   //��ά�����Ž��
	int row = SelectAll(sql, &azResult, &ncolumn);
	FreeResult(azResult);

	return row > 0;
}

// ��ȡ����
int Sqlite::GetRowCount(const char * sql)
{
	int ncolumn = 0;   //��ѯ�����ݵ�����
	char **azResult = NULL;   //��ά�����Ž��
	int row = SelectAll(sql, &azResult, &ncolumn);
	FreeResult(azResult);

	return row;
}

// ��ȡ���ֶ���ֵ
bool Sqlite::GetOneCol(const char * sql, char dst[])
{
	int ncolumn = 0;   //��ѯ�����ݵ�����
	char **azResult = NULL;   //��ά�����Ž��
	int row = SelectAll(sql, &azResult, &ncolumn);
	if (ncolumn == 0 || row == 0)
		return false;

	strcpy(dst, azResult[ncolumn]);
	FreeResult(azResult);

	return true;
}

// ��ѯȫ��[��������]
int Sqlite::SelectAll(const char* sql, char *** result, int* col, char** msg)
{
	int nrow = 0;       //��ѯ�����ݵ�����
	if (sqlite3_get_table(m_db, sql, result, &nrow, col, msg) != SQLITE_OK) { //��ѯ���ݿ�
		printf("SQL����: %s\n", sqlite3_errmsg(m_db));
	}; 
	return nrow;
}

// �ͷŲ�ѯ���
void Sqlite::FreeResult(char ** result)
{
	sqlite3_free_table(result);
}

// ִ��SQL
bool Sqlite::Exec(const char* sql)
{
	if (sqlite3_exec(m_db, sql, nullptr, nullptr, nullptr) != SQLITE_OK) {
		printf("SQL����: %s\n", sqlite3_errmsg(m_db));
		return false;
	}
	return true;
}

// �ر�
void Sqlite::Close()
{
	sqlite3_close(m_db);
}

// ��Ŀ
int Sqlite::Count()
{
	strcpy(m_sField, "COUNT(*)");
	return GetCol2Int();
}

// ��ȡһ����Ϣ
char** Sqlite::Get()
{
	MakeSelectSql();

	int row, col;
	return List(row, col);
}

// ��ȡ�����ֶ���Ϣ
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

// ��ȡ�����ֶ���Ϣ
int Sqlite::GetCol2Int(int dv)
{
	char** result = Get();
	int v = result ? atoi(result[0]) : dv;
	FreeResult();
	return v;
}

// ��ȡ�б�
char** Sqlite::List(int& row, int& col)
{
	MakeSelectSql();

	row = SelectAll(GetSql(), &m_sResult, &col);
	if (col == 0 || row == 0)
		return nullptr;

	return &m_sResult[col];
}

// �ͷ���Դ
void Sqlite::FreeResult()
{
	if (m_sResult) {
		FreeResult(m_sResult);
		m_sResult = nullptr;
	}
}
