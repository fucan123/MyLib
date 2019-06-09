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
	Parse();
}

Explode::~Explode()
{
}

char* Explode::GetValue(int index)
{
	if (index >= m_Count || index >= EXPLODE_MAX_LENGTH)
		return nullptr;

	return m_Value[index];
}

int Explode::GetValue2Int(int index, int default_value)
{
	char* value = GetValue(index);
	if (value == nullptr)
		return default_value;

	int num = 0;
	while (*value) {
		if (*value < '0' || *value > '9')
			break;
		num = num * 10 + (*value - '0');
		value++;
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
				//printf("Num:%d String:%c\n", num, *string);
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
	if (index >= EXPLODE_MAX_LENGTH || length >= EXPLODE_MAX_BYTE)
		return;

	for (int i = 0; i < length; i++) {
		m_Value[index][i] = start[i];
	}
	m_Value[index][length] = 0;
}
