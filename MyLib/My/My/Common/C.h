#pragma

// �����
int MyRand(int min, int max, int f=0);

// ����ַ���
void RandStr(char save[], int min_length, int max_length, int flag=0, int f=0);

// �ж��ļ�����
bool file_exists(const char* file);

// ��ȡ�ļ���С file=�ļ� ���� -1=������ ����Ϊ��С
int file_size(const char* file);

// ��ȡ�ı�����
int file_get_contents(const char* file, char* save, int save_size);

// д���ı�����
int file_put_contents(const char* file, const char* content);