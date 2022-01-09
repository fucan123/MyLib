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

// file=配置文件
FileConfig::FileConfig(const char* file)
{
	ReadConfig(file);
}

// 析构
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

// 读取配置文件 file=配置文件
bool FileConfig::ReadConfig(const char* file)
{
	if (IsReadOk()) // 已读取
		return true;

	OpenTextFile openTextFile;
	if (!openTextFile.Open(file)) { // 文件不存在
		SetReadFlag(FileConfigReadFlag::FileConfigReadFaild);
		return false;
	}

	int length = 0;
	char data[256];
	while (true) {
		length = openTextFile.GetLine(data, sizeof(data)); // 读取一行
		if (length == -1)
			break;

		ParseString(data, length); // 解析
	}

	SetReadFlag(FileConfigReadFlag::FileConfigReadOk); // 成功

	m_pValueCurrent = m_pValueLink;
	m_pValueMul = nullptr;
	return true;
}

// 是否读取配置成功
bool FileConfig::IsReadOk()
{
	return m_nReadFlag == FileConfigReadFlag::FileConfigReadOk;
}

// 获取值 key=键值
const char* FileConfig::GetValue(const char* key)
{
	return GetValue(key, 0);
}

// 获取值 key=键值 index=第几个
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

// 获取返回值 key=键值 count=返回数组长度
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

// 获取值 key=键值 dv=默认值
int FileConfig::GetValue2Int(const char* key)
{
	return GetValue2Int(key, 0, 0);
}

// 获取值 key=键值 index=第几个 dv=默认值
int FileConfig::GetValue2Int(const char* key, int index, int dv)
{
	const char* v = GetValue(key, index);
	if (v == nullptr)
		return  dv;

	return atoi(v);
}

// 打印所有数值
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

// 解析一行配置
void FileConfig::ParseString(char* data, int length)
{
	bool bIsMul = false; // 是否多行开始
	if (length == 0) {
		if (data[0] == '[' && strstr(data, "]:") != nullptr) {
			bIsMul = true;
			m_pValueMul = nullptr;
		}
		if (!bIsMul) // 注释
			return;
	}

	char finds[] = { '\n', '\r', '\t', '[', ']', ' '};
	::replace(data, finds, sizeof(finds), '\0');
	if (data[0] == 0) { // 没有数据
		m_pValueMul = nullptr;
		return;
	}
	if (strstr(data, "=") != nullptr) {
		m_pValueMul = nullptr;
	}

	if (m_pValueMul) { // 是多行的值
		size_t length = ::strlen(data);
		m_pValueMul->Value[m_pValueMul->ValueCount] = new char[length + 1];
		::strcpy_s(m_pValueMul->Value[m_pValueMul->ValueCount], length + 1, data);
		m_pValueMul->ValueCount++;
		return;
	}

	PFileConfigValue pFileConfigValue = new FileConfigValue;
	::memset(pFileConfigValue, 0, sizeof(FileConfigValue));
	if (bIsMul) { // 是多行开始键值
		::strcpy_s(pFileConfigValue->Key, sizeof(pFileConfigValue->Key), data); // 复制键值
		::rtrim(pFileConfigValue->Key, ':');
		m_pValueMul = pFileConfigValue;
	}
	else {
		Explode arr("=", data);
		if (arr.GetCount() == 2) { // 符合
			::strcpy_s(pFileConfigValue->Key, sizeof(pFileConfigValue->Key), arr[0]); // 键值

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

// 设置读取标志 0-还未读取配置文件 -1=配置文件不存在 1=已读取配置文件
void FileConfig::SetReadFlag(FileConfigReadFlag flag)
{
	m_nReadFlag = flag;
}
