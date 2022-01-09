#pragma once
#define SQL_MAX_LENGTH 1024

class Sql
{
private:
	bool m_bIsSetWhere = false;
protected:
	char   m_sTable[32];
	char   m_sField[64];
	char   m_sWhere[512];
	char   m_sOrder[32];
	char   m_sLimit[32];
	char*  m_sSql;
	char** m_sResult;
public:
	// Sql
	Sql();
	// ~Sql
	~Sql();
	// Init
	void Init(int flag=0);
	// ��ȡSQL���
	const char* GetSql();
	// ��ӡsql���
	void PrintSql();
public:
	// �������ݱ�
 	virtual Sql* Table(const char* table, const char* field=nullptr);
	// Where
	virtual Sql* OrWhere(const char* field, int value);
	// Where
	virtual Sql* OrWhere(const char* field, const char* str);
	// Where
	virtual Sql* OrWhere(const char* field, const char* sign, int value);
	// Where
	virtual Sql* Where(const char* field, int value);
	// Where
	virtual Sql* Where(const char* field, const char* str);
	// Where
	virtual Sql* Where(const char* field, const char* sign, int value);
	// Where
	virtual Sql* Where(const char* field, const char* sign, const char* str, bool is_num = false, bool is_and = true);
	// Order Asc
	virtual Sql* OrderAsc(const char* field);
	// Order Desc
	virtual Sql* OrderDesc(const char* field);
	// LIMIT
	virtual Sql* Limit(int start=-1, int end=-1);
public:
	// ��Ŀ
	virtual int Count() = 0;
	// ��ȡһ����Ϣ
	virtual char** Get() = 0;
	// ��ȡ�����ֶ���Ϣ
	virtual char* GetCol(char* dst=nullptr) = 0;
	// ��ȡ�����ֶ���Ϣ
	virtual int GetCol2Int(int dv=0) = 0;
	// ��ȡ�б�
	virtual char** List(int& row, int &col) = 0;
	// �ͷ���Դ
	virtual void FreeResult() = 0;
protected:
	// ������ѯSQL
	void MakeSelectSql();
};