#include "FileConfig.h"
#include "C.h"
#include "mystring.h"
#include "OpenTextFile.h"
#include "Explode.h"
#include <stdio.h>

// ...
FileConfig::FileConfig()
{
}

// file=�����ļ�
FileConfig::FileConfig(const char* file)
{
	ReadConfig(file);
}

// ����
FileConfig::~FileConfig()
{
	PFileConfigValue p = m_pValueLink;
	while (p) {
		for (int i = 0; i < p->ValueCount; i++) {
			delete[] p->Value[i];
		}

		PFileConfigValue pTmp = p;
		p = p->Next;
		delete pTmp;
	}
}

// ��ȡ�����ļ� file=�����ļ�
bool FileConfig::ReadConfig(const char* file)
{
	if (IsReadOk()) // �Ѷ�ȡ
		return true;

	OpenTextFile openTextFile;
	if (!openTextFile.Open(file)) { // �ļ�������
		SetReadFlag(FileConfigReadFlag::FileConfigReadFaild);
		return false;
	}

	int length = 0;
	char data[256];
	while (true) {
		length = openTextFile.GetLine(data, sizeof(data)); // ��ȡһ��
		if (length == -1)
			break;

		ParseString(data, length); // ����
	}

	SetReadFlag(FileConfigReadFlag::FileConfigReadOk); // �ɹ�

	m_pValueCurrent = m_pValueLink;
	m_pValueMul = nullptr;
	return true;
}

// �Ƿ��ȡ���óɹ�
bool FileConfig::IsReadOk()
{
	return m_nReadFlag == FileConfigReadFlag::FileConfigReadOk;
}

// ��ȡֵ key=��ֵ
const char* FileConfig::GetValue(const char* key)
{
	return GetValue(key, 0);
}

// ��ȡֵ key=��ֵ index=�ڼ���
const char* FileConfig::GetValue(const char* key, int index)
{
	if (key == nullptr)
		return nullptr;
	if (index >= MAX_FILE_CONF_VC)
		return nullptr;

	PFileConfigValue p = m_pValueLink;
	while (p) {
		if (streq(p->Key, key)) {
			return p->Value[index];
		}
		p = p->Next;
	}
	return nullptr;
}

// ��ȡ����ֵ key=��ֵ count=�������鳤��
char** FileConfig::GetValueMul(const char* key, int& count)
{
	count = 0;
	if (key == nullptr)
		return nullptr;

	PFileConfigValue p = m_pValueLink;
	while (p) {
		if (streq(p->Key, key)) {
			count = p->ValueCount;
			return p->Value;
		}
		p = p->Next;
	}
	return nullptr;
}

// ��ȡֵ key=��ֵ dv=Ĭ��ֵ
int FileConfig::GetValue2Int(const char* key)
{
	return GetValue2Int(key, 0, 0);
}

// ��ȡֵ key=��ֵ index=�ڼ��� dv=Ĭ��ֵ
int FileConfig::GetValue2Int(const char* key, int index, int dv)
{
	const char* v = GetValue(key, index);
	if (v == nullptr)
		return  dv;

	return atoi(v);
}

// ��ӡ������ֵ
void FileConfig::PrintAll()
{
	::printf("********************************************************\n");
	PFileConfigValue p = m_pValueLink;
	while (p) {
		if (p->ValueCount == 1) {
			::printf("%s=%s\n", p->Key, p->Value[0]);
		}
		else {
			::printf("%s:\n", p->Key);
			for (int i = 0; i < p->ValueCount; i++) {
				::printf("%s\n", p->Value[i]);
			}
		}
		::printf("--------\n");
		p = p->Next;
	}
	::printf("********************************************************\n");
}

// ����һ������
void FileConfig::ParseString(char* data, int length)
{
	bool bIsMul = false; // �Ƿ���п�ʼ
	if (length == 0) {
		if (data[0] == '[' && strstr(data, "]:") != nullptr) {
			bIsMul = true;
			m_pValueMul = nullptr;
		}
		if (!bIsMul) // ע��
			return;
	}

	char finds[] = { '\n', '\r', '\t', '[', ']', ' '};
	::replace(data, finds, sizeof(finds), '\0');
	if (data[0] == 0) { // û������
		m_pValueMul = nullptr;
		return;
	}
	if (strstr(data, "=") != nullptr) {
		m_pValueMul = nullptr;
	}

	if (m_pValueMul) { // �Ƕ��е�ֵ
		size_t length = ::strlen(data);
		m_pValueMul->Value[m_pValueMul->ValueCount] = new char[length + 1];
		::strcpy_s(m_pValueMul->Value[m_pValueMul->ValueCount], length + 1, data);
		m_pValueMul->ValueCount++;
		return;
	}

	PFileConfigValue pFileConfigValue = new FileConfigValue;
	::memset(pFileConfigValue, 0, sizeof(FileConfigValue));
	if (bIsMul) { // �Ƕ��п�ʼ��ֵ
		::strcpy_s(pFileConfigValue->Key, sizeof(pFileConfigValue->Key), data); // ���Ƽ�ֵ
		::rtrim(pFileConfigValue->Key, ':');
		m_pValueMul = pFileConfigValue;
	}
	else {
		Explode arr("=", data);
		if (arr.GetCount() == 2) { // ����
			::strcpy_s(pFileConfigValue->Key, sizeof(pFileConfigValue->Key), arr[0]); // ��ֵ

			Explode v("|", arr[1]);
			for (int i = 0; i < v.GetCount(); i++) {
				size_t length = ::strlen(v[i]);
				pFileConfigValue->Value[i] = new char[length + 1];
				::strcpy_s(pFileConfigValue->Value[i], length + 1, v[i]);
			}
			pFileConfigValue->ValueCount = v.GetCount();
		}
		else {
			delete pFileConfigValue;
			pFileConfigValue = nullptr;
		}
	}

	if (pFileConfigValue) {
		if (m_pValueLink == nullptr) {
			m_pValueLink = pFileConfigValue;
			m_pValueCurrent = pFileConfigValue;
		}
		else {
			m_pValueCurrent->Next = pFileConfigValue;
			m_pValueCurrent = pFileConfigValue;
		}
	}
}

// ���ö�ȡ��־ 0-��δ��ȡ�����ļ� -1=�����ļ������� 1=�Ѷ�ȡ�����ļ�
void FileConfig::SetReadFlag(FileConfigReadFlag flag)
{
	m_nReadFlag = flag;
}
