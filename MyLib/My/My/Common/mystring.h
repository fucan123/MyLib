#pragma once

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
// 替换字符
char* replace(char* string, char find=0, char rep=0);
// 复制字符串
char* copy_str(char* start, char* end);
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
获取时间
*/
int gettimeofday(struct timeval* tv);

/*
获取毫秒
 */
__int64 getmillisecond();

/* 获取日期 */
int getday(int* hour);