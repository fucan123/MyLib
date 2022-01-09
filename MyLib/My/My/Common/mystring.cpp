#include "mystring.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>

// 比较两个字符串是否相等
bool streq(const char* str_1, const char* str_2)
{
	while (true) {
		if (*str_1 != *str_2) {
			return false;
		}
		else {
			if (*str_1 == 0)
				break;
		}

		str_1++;
		str_2++;
	}
	return true;
}

//比较两个字符串是否相等, len=比较的长度
bool strneq(const char* str_1, const char* str_2, unsigned int len) { 
	for (unsigned int i = 0; i < len; i++) {
		if (str_1[i] != str_2[i]) {
			return false;
		}
	}
	return true;
}

// 十六进制转成数字
int hextoi32(const char* data)
{
	if (data == nullptr)
		return 0;

	if (data[0] == '0') {
		if (data[1] == 'x' || data[1] == 'X')
			data += 2;
	}
	//printf("%s\n", data);
	int value = 0;
	while (*data) {
		char ch = *data;
		//printf("%c ", ch);
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
		}
	   // printf("%d\n", ch);
		value = value * 0x10 + ch;
		data++;
	}
	return value;
}

char* trim(char* string, char ch)
{
	ltrim(string, ch);
	rtrim(string, ch);
	return string;
}

char* ltrim(char* string, char ch)
{
	int left = 0;
	char* tmp = string;
	while (*tmp) {
		bool find = false;
		if (ch > 0) {
			find = ch == *tmp;
		}
		else {
			find = ' ' == *tmp || '\r' == *tmp || '\n' == *tmp || '\t' == *tmp;
		}
		if (!find) {
			break;
		}

		left++;
		tmp++;
	}
	if (left > 0) {
		int i;
		tmp = string + left;
		for (i = 0; *tmp; i++, tmp++) {
			string[i] = *tmp;
		}
		string[i] = 0;
	}
	return string;
}

char* rtrim(char* string, char ch)
{
	int length = strlen(string);
	for (int i = length - 1; i >= 0; i--) {
		bool find = false;
		if (ch > 0) {
			find = ch == string[i];
		}
		else {
			find = ' ' == string[i] || '\r' == string[i] || '\n' == string[i] || '\t' == string[i];
		}
		if (!find) {
			break;
		}
		string[i] = 0;
	}
	return string;
}

// 替换字符 把find替换成rep
char* replace(char* string, char find, char rep)
{
	if (rep) {
		for (char* p = string; *p; p++) {
			bool eq = false;
			if (find > 0) {
				eq = find == *p;
			}
			else {
				eq = '\r' == *p || '\n' == *p || '\t' == *p;
			}
			if (eq) {
				*p = rep;
			}
		}
	}
	else {
		for (char* p = string; *p; p++) {

			bool eq = false;
			if (find > 0) {
				eq = find == *p;
			}
			else {
				eq = '\r' == *p || '\n' == *p || '\t' == *p;
			}
			if (eq) {
				int i = 0;
				char* p2 = p + 1;
				for (; p2[i]; i++) {
					p[i] = p2[i];
				}
				p[i] = p2[i];

				p--;
			}
		}
	}
	return string;
}

// 替换字符 finds=要替换的数组 finds_length=要替换的数组长度
char* replace(char* string, char finds[], int finds_length, char rep)
{
	if (rep) {
		for (char* p = string; *p; p++) {
			bool eq = false;
			for (int i = 0; i < finds_length; i++) {
				if (finds[i] == *p) {
					eq = true;
					break;
				}
			}
			if (eq) {
				*p = rep;
			}
		}
	}
	else {
		for (char* p = string; *p; p++) {

			bool eq = false;
			for (int i = 0; i < finds_length; i++) {
				if (finds[i] == *p) {
					eq = true;
					break;
				}
			}
			if (eq) {
				int i = 0;
				char* p2 = p + 1;
				for (; p2[i]; i++) {
					p[i] = p2[i];
				}
				p[i] = p2[i];

				p--;
			}
		}
	}
	return string;
}

char* copy_str(char* start, char* end) {
	int len = end - start;
	if (len <= 0) {
		return NULL;
	}
	char* str = (char*)malloc(len + 1);
	char* s = start;
	char* tmp = str;
	for (; s < end; s++, tmp++) {
		if (*s == '\\') {
			if (s + 1 < end) {
				*tmp = *++s;
			}
		}
		else {
			*tmp = *s;
		}
	}
	*tmp = 0;

	return str;
}

// 获取中间的字符
char* get_mid_str(char* res, const char* source, const char* start, const char* end, bool fmt)
{
	res[0] = 0;
	const char* s = strstr(source, start);
	if (!s) {
		return res;
	}
	s += strlen(start);

	const char* e = 0;
	if (end) {
		e = strstr(source, end);
	}

	int i = 0;
	for (; *s; s++, i++) {
		if (e && s == e)
			break;

		res[i] = *s;
	}
	res[i] = 0;

	if (fmt) {
		trim(res);
	}

	return res;
}

//查找字符串need在text出现的位置[0起始], start=开始搜索位置, end=结束搜索位置
int strpos(char* text, char* need, unsigned int start, unsigned int end) { 
	if (end > 0) {
		end += (unsigned int)text + 1;
	}
	int need_len = strlen(need);
	bool find = false;
	char* ptr = text + start;
	while (*ptr) {
		if (end > 0 && ((unsigned int)ptr + need_len) > end) {
			break;
		}
		if (strneq(ptr, need, need_len)) {
			find = true;
			break;
		}
		ptr++;
	}
	return find ? ptr - text : -1;
}

char* substr(char* text, int start, int end, int length)
{
	length = length < 0 ? strlen(text) : length;
	int i = 0;
	if (start < 0) // 开始位置为负数
		start += length;
	if (end < 0)   // 结束位置为负数
		end += length;
	if (end == 0)
		end = length; // 结束位置为0 默认为字符串长度
	if (start < 0)
		start = 0;
	if (end < 0)
		end = 0;
	if (end > length)
		end = length;

	for (int i = start,j = 0; i < end; i++,j++) {
		text[j] = text[i];
	}
	int zero_pos = end - start;
	text[zero_pos < 0 ? 0 : zero_pos] = 0;
end:
	return text;
}

/*
* 把整型数字转成utf8字符串
* dest 字符串保存地址
* num 数字[数组]
* len 数组长度
*/
char* int2utf8(char* dest, int* num, int len) {
	int index = 0;
	for (int i = 0; i < len; i++) {
		if (num[i] < 128) {
			dest[index] = num[i] & 0xff;
			index++;
		}
		else {
			dest[index] = (num[i] >> 16) & 0xff;
			dest[index + 1] = (num[i] >> 8) & 0xff;
			dest[index + 2] = (num[i]) & 0xff;
			index += 3;
		}
	}
	dest[index] = 0;
	return dest;
}

/*
网络字端转成数字
*/
int ctoi32(const char * data)
{
	int v = 0;
	v |= (data[0] << 24) & 0xff000000;
	v |= (data[1] << 16) & 0x00ff0000;
	v |= (data[2] << 8) & 0x0000ff00;
	v |= (data[3] << 0) & 0x000000ff;
	return v;
}

/*
网络字端转成数字
*/
__int64 ctoi64(const char* data)
{
	__int64 v = 0;
	for (int i = 0; i < 8; i++) {
		//printf("%02x ", data[i] & 0xff);
		v |= (data[i] & 0xff) << ((7 - i) * 8);
	}
	//printf("\n");
	//printf(" --> %lld、%d\n", v, 123);
	return v;
}

/*
 复制int32到内存
*/
char* copy_i32(char* dst, int v)
{
	for (int i = 0; i < 4; i++) { 
		dst[i] = (v >> (i * 8)) & 0xff;
	}
	return dst;
}

/*
 复制int32（转成网络字节）到内存
*/
char* copy_i32_net(char* dst, int v)
{
	for (int i = 0; i < 4; i++) { 
		dst[i] = (v >> ((3 - i) * 8)) & 0xff;
	}
	return dst;
}

/*
 复制int64到内存
*/
char* copy_i64(char* dst, __int64 v)
{
	for (int i = 0; i < 8; i++) {
		dst[i] = (v >> (i * 8)) & 0xff;
	}
	return dst;
}

/*
 复制int64（转成网络字节）到内存
*/
 char* copy_i64_net(char* dst, __int64 v)
 {
	 for (int i = 0; i < 8; i++) {
		 dst[i] = (v >> ((7 - i) * 8)) & 0xff;
	 }
	 return dst;
 }

 /*
utf8转成gb2312
*/
char* Utf82GB2312(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

/*
gb2312转成utf8
*/
char* GB23122Utf8(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

 /*
获取时间
*/
int gettimeofday(struct timeval* tv)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;
	static int tzflag;

	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;
	#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
		/*converting file time to unix epoch*/
		tmpres /= 10;  /*convert into microseconds*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	return 0;
}

/*
获取毫秒
*/
__int64 getmillisecond()
{
	timeval t;
	gettimeofday(&t);
	return ((__int64)(t.tv_sec) * 1000) + (t.tv_usec / 1000);
}

/* 获取日期 */
int getday(int* hour)
{
	time_t now_time = time(nullptr);
	tm t;
	localtime_s(&t, &now_time);

	if (hour) {
		*hour = t.tm_hour;
	}

	return t.tm_mday;
}

/* 宽字符转字符 */
char* wchar2char(const wchar_t* str)
{
	char * m_char;
	int len = WideCharToMultiByte(CP_ACP, 0, str, wcslen(str), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, str, wcslen(str), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	return m_char;
}

/* 时间转成字符串 */
char* time2str(char save[], int s, int utc)
{
	tm t;
	time_t nm = s + utc * 3600;
	gmtime_s(&t, &nm);
	sprintf(save, "%d-%02d-%02d %02d:%02d:%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
	return save;
}

/* 转成小写 */
char* strtolower(char* str)
{
	for (int i = 0; str[i]; i++) {
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] += 32;
	}
	return str;
}


/* 转成大写 */
char* strtoupper(char* str)
{
	for (int i = 0; str[i]; i++) {
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] -= 32;
	}
	return str;
}

/* 是否数字 */
bool is_number(const char* str)
{
	if (*str == '-') str++;
	for (; *str; str++) {
		if (*str < '0' || *str > '9')
			return false;
	}
	return true;
}

/* 是否十六进制 */
bool is_hex(const char * str)
{
	if (*str == '0') {
		str++;
		if (*str != 'x' && *str != 'X')
			return false;

		str++;
	}
	for (; *str; str++) {
		if (*str < 'A' || *str > 'z')
			return false;
		if (*str > 'Z' && *str < 'a')
			return false;
	}
	return true;
}

/*  十六进制转整型 length=转换字符串之长度 */
int hex2int(const char* str, int length)
{
	if (str == nullptr)
		return 0;

	if (str[0] == '0') {
		if (str[1] == 'x' || str[1] == 'X')
			str += 2;
	}

	int i = 0;
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
		if (length > 0 && (++i >= length))
			break;

		str++;
	}
	return num;
}

/* 十六进制字符转成字符 */
int hexstr2char(char* save, const char* str, int length)
{
	if (length & 0x01) {
		return 0;
	}

	length /= 2;
	for (int i = 0; i < length; i++) {
		save[i] = hex2int(&str[i * 2], 2);
	}
	save[length] = 0;

	return length;
}

/* 转成十六进制 return=save大小 */
int char2hex(char* save, const char* str, int length)
{
	for (int i = 0, j = 0; i < length; i++, j += 2) {
		char tmp[3];
		sprintf_s(tmp, "%02x", str[i]&0xff);
		save[j] = tmp[0];
		save[j + 1] = tmp[1];
	}
	save[length * 2] = 0;

	return length * 2;
}
