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
	// 获取SQL语句
	const char* GetSql();
	// 打印sql语句
	void PrintSql();
public:
	// 设置数据表
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
	// 数目
	virtual int Count() = 0;
	// 获取一条信息
	virtual char** Get() = 0;
	// 获取单个字段信息
	virtual char* GetCol(char* dst=nullptr) = 0;
	// 获取单个字段信息
	virtual int GetCol2Int(int dv=0) = 0;
	// 获取列表
	virtual char** List(int& row, int &col) = 0;
	// 释放资源
	virtual void FreeResult() = 0;
protected:
	// 制作查询SQL
	void MakeSelectSql();
};