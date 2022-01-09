#include "Sql.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Sql
Sql::Sql()
{
    m_sSql = new char[SQL_MAX_LENGTH];
    Init(0);
}

// ~Sql
Sql::~Sql()
{
    if (m_sSql) {
        delete[] m_sSql;
        m_sSql = nullptr;
    }
}

void Sql::Init(int flag)
{
    m_bIsSetWhere = false;
    memset(m_sTable, 0, flag==0 ? sizeof(m_sTable) : 1);
    memset(m_sField, 0, flag==0 ? sizeof(m_sField) : 1);
    memset(m_sWhere, 0, flag==0 ? sizeof(m_sWhere) : 1);
    memset(m_sOrder, 0, flag==0 ? sizeof(m_sOrder) : 1);
    memset(m_sLimit, 0, flag==0 ? sizeof(m_sLimit) : 1);
    memset(m_sSql,   0, flag==0 ? SQL_MAX_LENGTH   : 1);
    m_sResult = nullptr;
}

// 获取SQL
const char* Sql::GetSql()
{
    return m_sSql;
}

// 打印sql
void Sql::PrintSql()
{
    ::printf("[SQL预览]: %s\n", m_sSql);
}

Sql* Sql::Table(const char* table, const char* field)
{
    Init(1);

    strcpy(m_sTable, table);
    if (!field) {
        strcpy(m_sField, "*");
    }
    else {
        strcpy(m_sField, field);
    }
    return this;
}

// OrWhere
Sql* Sql::OrWhere(const char* field, int value)
{
    return OrWhere(field, "=", value);
}

// OrWhere
Sql* Sql::OrWhere(const char* field, const char* str)
{
    return Where(field, "=", str, false, false);
}

// WHERE
Sql* Sql::OrWhere(const char* field, const char* sign, int value)
{
    char str[16];
    sprintf_s(str, "%d", value);
    return Where(field, sign, str, true, false);
}

// Where
Sql* Sql::Where(const char* field, int value)
{
    return Where(field, "=", value);
}

// WHERE
Sql* Sql::Where(const char* field, const char* str)
{
    return Where(field, "=", str);
}

// WHERE
Sql* Sql::Where(const char* field, const char* sign, int value)
{
    char str[16];
    sprintf_s(str, "%d", value);
    return Where(field, sign, str, true);
}

// WHERE
Sql* Sql::Where(const char* field, const char* sign, const char* str, bool is_num, bool is_and)
{
    if (!m_bIsSetWhere) {
        m_bIsSetWhere = true;
        strcat(m_sWhere, "WHERE ");
    }
    else {
        if (is_and) {
            strcat(m_sWhere, " AND ");
        }
        else {
            strcat(m_sWhere, " OR ");
        }
        
    }
    strcat(m_sWhere, field);
    strcat(m_sWhere, sign);
    if (!is_num) {
        strcat(m_sWhere, "\"");
    }
    strcat(m_sWhere, str);
    if (!is_num) {
        strcat(m_sWhere, "\"");
    }

    return this;
}

// Order Asc
Sql* Sql::OrderAsc(const char* field)
{
    sprintf_s(m_sOrder, " ORDER BY %s ASC", field);
    return this;
}

// Order Desc
Sql* Sql::OrderDesc(const char* field)
{
    sprintf_s(m_sOrder, " ORDER BY %s DESC", field);
    return this;
}

// LIMIT
Sql* Sql::Limit(int start, int end)
{
    if (start > -1 && end > -1) {
        sprintf_s(m_sLimit, " LIMIT %d,%d", start, end);
    }
    else if (start > -1) {
        sprintf_s(m_sLimit, " LIMIT %d", start);
    }
    
    return this;
}

// 制作查询sql
void Sql::MakeSelectSql()
{
    sprintf(m_sSql, "SELECT %s FROM `%s` %s%s%s", m_sField, m_sTable, m_sWhere, m_sOrder, m_sLimit);
}