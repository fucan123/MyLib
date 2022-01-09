#pragma

// 随机数
int MyRand(int min, int max, int f=0);

// 随机字符串
void RandStr(char save[], int min_length, int max_length, int flag=0, int f=0);

// 判断文件存在
bool file_exists(const char* file);

// 获取文件大小 file=文件 返回 -1=不存在 其他为大小
int file_size(const char* file);

// 读取文本内容
int file_get_contents(const char* file, char* save, int save_size);

// 写入文本内容
int file_put_contents(const char* file, const char* content);