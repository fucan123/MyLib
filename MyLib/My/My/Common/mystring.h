#pragma once

// 比较两个字符串是否相等
bool streq(const char* str_1, const char* str_2);
// 比较两个字符串是否相等, len=比较的长度
bool strneq(const char* str_1, const char* str_2, unsigned int len);
// 十六进制转成数字
int hextoi32(const char* data);
// 去除两边字符
char* trim(char* string, char ch = 0);
// 去除左边字符
char* ltrim(char* string, char ch = 0);
// 去除右边字符
char* rtrim(char* string, char ch=0);
// 替换字符 把find替换成rep
char* replace(char* string, char find=0, char rep=0);
// 替换字符 finds=要替换的数组 finds_length=要替换的数组长度
char* replace(char* string, char finds[], int finds_length, char rep=0);
// 复制字符串
char* copy_str(char* start, char* end);
// 获取中间的字符
char* get_mid_str(char* res, const char* source, const char* start, const char* end=0, bool fmt=true);
// 查找字符串need在text出现的位置[0起始], start=开始搜索位置, end=结束搜索位置
int strpos(char* text, char* need, unsigned int start = 0, unsigned int end = 0);
/*
  截取字符串
  text=要截取的字符串
  start=开始位置(负数从末尾开始截取)
  end=截取长度(0为字符串长度, 负数从末尾开始计算)
  length=字符串长度(默认自动计算)
*/
char* substr(char* text, int start, int end, int length=-1);
/*
* 把整型数字转成utf8字符串
* dest 字符串保存地址
* num 数字[数组]
* len 数组长度
*/
char* int2utf8(char* dest, int* num, int len);

/*
网络字端转成数字
*/
int ctoi32(const char* data);

/*
网络字端转成数字
*/
__int64 ctoi64(const char* data);

/*
 复制int32到内存
*/
char* copy_i32(char* dst, int v);

/*
 复制int32（转成网络字节）到内存
*/
char* copy_i32_net(char* dst, int v);

/*
 复制int64到内存
*/
char* copy_i64(char* dst, __int64 v);

/*
 复制int64（转成网络字节）到内存
*/
char* copy_i64_net(char* dst, __int64 v);

/*
utf8转成gb2312
*/
char* Utf82GB2312(const char* utf8);

/*
gb2312转成utf8
*/
char* GB23122Utf8(const char* utf8);

/*
获取时间
*/
int gettimeofday(struct timeval* tv);

/*
获取毫秒
 */
__int64 getmillisecond();

/* 获取日期 */
int getday(int* hour);

/* 宽字符转字符 */
char* wchar2char(const wchar_t* str);

/* 时间转成字符串 */
char* time2str(char save[], int s, int utc=0);

/* 转成小写 */
char* strtolower(char* str);
/* 转成大写 */
char* strtoupper(char* str);

/* 是否数字 */
bool is_number(const char* str);

/* 是否十六进制 */
bool is_hex(const char* str);

/* 十六进制转整型 length=转换字符串之长度 */
int hex2int(const char* str, int length=0);

/* 十六进制字符转成字符 */
int hexstr2char(char* save, const char* str, int length);

/* 转成十六进制 return=save长度 */
int char2hex(char* save, const char* str, int length);