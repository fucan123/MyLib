#pragma once

// �Ƚ������ַ����Ƿ����
bool streq(const char* str_1, const char* str_2);
// �Ƚ������ַ����Ƿ����, len=�Ƚϵĳ���
bool strneq(const char* str_1, const char* str_2, unsigned int len);
// ʮ������ת������
int hextoi32(const char* data);
// ȥ�������ַ�
char* trim(char* string, char ch = 0);
// ȥ������ַ�
char* ltrim(char* string, char ch = 0);
// ȥ���ұ��ַ�
char* rtrim(char* string, char ch=0);
// �滻�ַ� ��find�滻��rep
char* replace(char* string, char find=0, char rep=0);
// �滻�ַ� finds=Ҫ�滻������ finds_length=Ҫ�滻�����鳤��
char* replace(char* string, char finds[], int finds_length, char rep=0);
// �����ַ���
char* copy_str(char* start, char* end);
// ��ȡ�м���ַ�
char* get_mid_str(char* res, const char* source, const char* start, const char* end=0, bool fmt=true);
// �����ַ���need��text���ֵ�λ��[0��ʼ], start=��ʼ����λ��, end=��������λ��
int strpos(char* text, char* need, unsigned int start = 0, unsigned int end = 0);
/*
  ��ȡ�ַ���
  text=Ҫ��ȡ���ַ���
  start=��ʼλ��(������ĩβ��ʼ��ȡ)
  end=��ȡ����(0Ϊ�ַ�������, ������ĩβ��ʼ����)
  length=�ַ�������(Ĭ���Զ�����)
*/
char* substr(char* text, int start, int end, int length=-1);
/*
* ����������ת��utf8�ַ���
* dest �ַ��������ַ
* num ����[����]
* len ���鳤��
*/
char* int2utf8(char* dest, int* num, int len);

/*
�����ֶ�ת������
*/
int ctoi32(const char* data);

/*
�����ֶ�ת������
*/
__int64 ctoi64(const char* data);

/*
 ����int32���ڴ�
*/
char* copy_i32(char* dst, int v);

/*
 ����int32��ת�������ֽڣ����ڴ�
*/
char* copy_i32_net(char* dst, int v);

/*
 ����int64���ڴ�
*/
char* copy_i64(char* dst, __int64 v);

/*
 ����int64��ת�������ֽڣ����ڴ�
*/
char* copy_i64_net(char* dst, __int64 v);

/*
utf8ת��gb2312
*/
char* Utf82GB2312(const char* utf8);

/*
gb2312ת��utf8
*/
char* GB23122Utf8(const char* utf8);

/*
��ȡʱ��
*/
int gettimeofday(struct timeval* tv);

/*
��ȡ����
 */
__int64 getmillisecond();

/* ��ȡ���� */
int getday(int* hour);

/* ���ַ�ת�ַ� */
char* wchar2char(const wchar_t* str);

/* ʱ��ת���ַ��� */
char* time2str(char save[], int s, int utc=0);

/* ת��Сд */
char* strtolower(char* str);
/* ת�ɴ�д */
char* strtoupper(char* str);

/* �Ƿ����� */
bool is_number(const char* str);

/* �Ƿ�ʮ������ */
bool is_hex(const char* str);

/* ʮ������ת���� length=ת���ַ���֮���� */
int hex2int(const char* str, int length=0);

/* ʮ�������ַ�ת���ַ� */
int hexstr2char(char* save, const char* str, int length);

/* ת��ʮ������ return=save���� */
int char2hex(char* save, const char* str, int length);