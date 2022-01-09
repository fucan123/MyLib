#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <io.h>
#include "C.h"

// �����
int MyRand(int min, int max, int f)
{
	if (min == max)
		return max;

	srand(GetTickCount() + f);

	if (min > max) {
		int tmp = min;
		min = max;
		max = tmp;
	}

	int v = 1;
	int r = rand();
	if (r == 0)
		v = 0;

	return r % (max - min + v) + min;
}

// ����ַ���
void RandStr(char save[], int min_length, int max_length, int flag, int f)
{
	int length = MyRand(min_length, max_length, f);
	for (int i = 0; i < length; i++) {
		int v = flag;
		if (v != 0 && v != 1) {
			//printf("flag:%d\n", flag);
			v = MyRand(0, 1, i + f * i);
		}
			
		if (v == 0)
			save[i] = MyRand('a', 'z', i + f * i);
		else if (v == 1)
			save[i] = MyRand('A', 'Z', i + f * i);
	}
	save[length] = 0;
}

// �ж��ļ�����
bool file_exists(const char* file)
{
	return _access(file, 0) == 0;
}

// ��ȡ�ļ���С file=�ļ� ���� -1=������ ����Ϊ��С
int file_size(const char* file)
{
	FILE* f = fopen(file, "r");
	if (!f)
		return -1;

	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fclose(f);

	return size;
}

// ��ȡ�ı�����
int file_get_contents(const char* file, char* save, int save_size)
{
	save[0] = 0;

	FILE* f = fopen(file, "r+");
	if (!f)
		return 0;

	int length = 0;
	char data[255];
	while (!feof(f)) {
		if (fgets(data, sizeof(data), f)) {
			int len = strlen(data);
			if (length + len >= save_size)
				break;

			strcat(save + length, data);
			length += len;

		}
	}

	fclose(f);
	return length;
}

// д���ı�����
int file_put_contents(const char* file, const char* content)
{
	FILE* f = fopen(file, "w+");
	if (!f)
		return 0;

	int length = fputs(content, f);
	fclose(f);

	return length;
}
