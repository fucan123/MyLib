#pragma once

// ���֧��shuzi
#define MAX_WORD_LENGTH 10

class SeeImgNum
{
public:
	// ...
	SeeImgNum();

	// ...
	~SeeImgNum();

	// pixels=����ָ�� width=��� height=�߶�
	SeeImgNum(int* pixels, int width, int height, bool opengl, int byte = 4);

	// ����������Ϣ pixels=����ָ�� width=��� height=�߶�
	void SetPixels(int* pixels, int width, int height, bool opengl, int byte=4);

	// ������ģ models=��ģ���� width=��ģ�� height=��ģ��
	void SetModels(int* models, int width, int height);

	// ������ģ model=��ģ num=��ģ��Ӧ��ֵ
	void SetModel(int* model, int num);

	// ������ģ��� width=��ģ�� height=��ģ��
	void SetModelWidthHeight(int width, int height);

	// ��ȡ������Ϣ
	int  GetPixel(int x, int y);

	// ��ȡ����
	int GetNum(int color, int diff, int d_v=0);

	// ��ȡ����
	int GetText(char* text, int color, int diff, int* pnum=nullptr);

	// ��������Ϣ
	int FindWords(int color, int diff);

	// ��ӡ��
	void PrintWords(int color, int diff);

	// �����Ƿ����Ҫ�� pixel=ԭ����ԭɫ color=�Ƚϵ���ɫֵ diff=����Ĳ�ֵ
	bool IsThePixel(int pixel, int color, int diff);
private:
	// ƥ����ģ, ����ƥ������
	int PregModels(int word_index, int model_index, int color, int diff);
	// ��ȡ��ģ����
	int* GetModels(int index);
private:
	// �Ƿ�opengl ��Ϊ��������
	bool m_bOpengl = false;
	// ����������Ϣ����
	int* m_pPixels = nullptr;
	// ���
	int m_nWidth = 0;
	// �߶�
	int m_nHeight = 0;
	// �ڴ������
	int m_nMallocWidth = 0;
	// �ڴ����߶�
	int m_nMallocHeight = 0;
private:
	// ��ģ
	int* m_Models[10];
	// �ҵ���������
	int  m_nFindWords = 0;
	// ��ģ���
	int  m_nModelWidth = 0;
	// ��ģ�߶�
	int  m_nModelHeight = 0;
	// ����Ϣ
	struct {
		int StartX; // ��ʼλ��X
		int EndX;   // ����λ��X
		int StartY; // ��ʼλ��Y
		int EndY;   // ����λ��Y
	} m_WordInfos[MAX_WORD_LENGTH];
};