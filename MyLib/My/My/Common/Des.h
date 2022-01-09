#define _CRT_SECURE_NO_WARNINGS

#include <memory.h>
#include <stdio.h>
#include <conio.h>

// DES加密
int  DesEncrypt(char* out, const char* key, const char* text, int text_length);
int  DesDecrypt(char* out, const char* key, const char* text, int text_length, bool hex = false);
static void Des_Run(char Out[16], const char* In, int InLength, bool decode=false);//核心加密算法
static void SetKey(const char Key[8]);// 设置密钥
static void F_func(bool In[32], const bool Ki[48]);// f 函数
static void S_func(bool Out[32], const bool In[48]);// S 盒代替
static void Transform(bool *Out, bool *In, const char *Table, int len);// 变换
static void Xor(bool *InA, const bool *InB, int len);// 异或
static void RotateL(bool *In, int len, int loop);// 循环左移
static void ByteToBit(bool *Out, const char *In, int bits);// 字节组转换成位组
static void ShowInHex(const bool In[64]);     //以16进制输出
static char Hex2Char(char ch, char ch2);
static char Char2Num(char ch);
