#pragma once
#include "Sql.h"
#include "sqlite3.h"

class Sqlite: public Sql
{
public:
	// ...
	Sqlite(const char* db_name=nullptr);
	// �����ݿ�
	bool Open(const char* db_name = nullptr);
	// ���ݱ��Ƿ����
	bool TableIsExists(const char* table);
	// ��ȡ����
	int  GetRowCount(const char* sql);
	// ��ȡ���ֶ���ֵ
	bool GetOneCol(const char* sql, char dst[]);
	// ��ѯȫ��[��������]
	int  SelectAll(const char* sql, char*** result, int* col, char** msg=nullptr);
	// �ͷŲ�ѯ���
	void FreeResult(char** result);
	// ִ��SQL
	bool Exec(const char* sql);
	// �ر�
	void Close();
public:
	// ��Ŀ
	virtual int Count();
	// ��ȡһ����Ϣ
	virtual char** Get();
	// ��ȡ�����ֶ���Ϣ
	virtual char* GetCol(char* dst=nullptr);
	// ��ȡ�����ֶ���Ϣ
	virtual int GetCol2Int(int dv=0);
	// ��ȡ�б�
	virtual char** List(int& row, int& col);
	// �ͷ���Դ
	virtual void FreeResult();
protected:
	// ���ݿ���
	sqlite3* m_db = nullptr;
};