#include "Explode.h"
#include <stdio.h>

Explode::Explode(const char  separator, const char* string)
{
	m_SeparatorCh[0] = separator;
	m_SeparatorCh[1] = 0;
	m_Separator = m_SeparatorCh;
	m_String = string;
	Parse();
}

Explode::Explode(const char* separator, const char* string)
{
	m_Separator = separator;
	m_String = string;

	for (int i = 0; i < EXPLODE_MAX_LENGTH; i++) { // 分配可用内存
		m_Value[i] = new char[EXPLODE_MAX_BYTE];
	}
	Parse();
}

Explode::~Explode()
{
	for (int i = 0; i < EXPLODE_MAX_LENGTH; i++) {
		delete[] m_Value[i];
	}
}

char* Explode::GetValue(int index)
{
	if (index >= m_Count || index >= EXPLODE_MAX_LENGTH)
		return nullptr;

	return m_Value[index];
}

int Explode::GetValue2Int(int index, int default_value, bool is_trim)
{
	char* str = GetValue(index);
	if (str == nullptr)
		return default_value;

	if (is_trim) {
		while (*str == ' ') str++;
	}
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
		return HexToInt(str);

	return DecToInt(str);
}

int Explode::GetHexValue2Int(int index, int default_value, bool is_trim)
{
	char* str = GetValue(index);
	if (str == nullptr)
		return default_value;

	if (is_trim) {
		while (*str == ' ') str++;
	}

	return HexToInt(str);
}

int Explode::DecToInt(const char* str, int default_value)
{
	if (str == nullptr)
		return default_value;

	bool sign = false;
	if (*str == '-') {
		str++;
		sign = true;
	}

	int num = 0;
	while (*str) {
		if (*str < '0' || *str > '9')
			break;

		num = num * 10 + (*str - '0');
		str++;
	}
	return sign ? num*-1 : num;
}

int Explode::HexToInt(const char* str, int default_value)
{
	if (str == nullptr)
		return default_value;

	if (str[0] == '0') {
		if (str[1] == 'x' || str[1] == 'X')
			str += 2;
	}

	int num = 0;
	while (*str) {
		char ch = *str;
		if (ch >= '0' && ch <= '9') {
			ch = ch - '0';
		}
		else if (ch >= 'A' && ch <= 'F') {
			ch = ch - 'A' + 0x0A;
		}
		else if (ch >= 'a' && ch <= 'f') {
			ch = ch - 'a' + 0x0a;
		}
		else {
			ch = 0;
			break;
		}

		num = num * 0x10 + ch;
		str++;
	}
	return num;
}

char* Explode::operator[](int index)
{
	return GetValue(index);
}

int Explode::GetCount()
{
	return m_Count;
}

void Explode::Parse()
{
	m_Count = Start();
}

int Explode::Start()
{
	if (!m_String)
		return 0;

	int num = 0;
	const char* separator = m_Separator;
	const char* string = m_String;
	const char* start =  string;
	const char* end = nullptr;
	while (*string) {
		//printf("String:%c\n", *string);
		bool eq = false;
		separator = m_Separator;
		if (*string == *separator) {
			end = string;
			if (*++separator) {
				string++;
			}
			//printf("OK\n");
			eq = true;
			const char* tmp = string;
			while (*separator) {
				//printf("separator:%c\n", *separator);
				if (*string != *separator) {
					eq = false;
					break;
				}
				if (*++separator) {
					string++;
				}
			}
			if (eq) {
				Copy(num, start, end);
				start = string + 1;
				num++;
				//printf("Num:%d String:%s\n", num, string);
			}
			else {
				string = tmp;
			}
		}
		string++;
	}
	Copy(num, start, string);
	return num + 1;
}

void Explode::Copy(int index, const char* start, const char *end)
{
	int length = end - start;
	if (index >= EXPLODE_MAX_LENGTH)
		return;
	if (length >= EXPLODE_MAX_BYTE)
		length = EXPLODE_MAX_BYTE - 1;

	for (int i = 0; i < length; i++) {
		m_Value[index][i] = start[i];
	}
	m_Value[index][length] = 0;
}
