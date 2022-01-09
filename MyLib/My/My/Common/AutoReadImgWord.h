#pragma once

// ���֧��shuzi
#define MAX_WORD_LENGTH 10
#define MAX_PWL_LENGTH  8

typedef struct __parse_word_info__
{
	int ln; // �б�����
	struct {
		int x;     // ��ʼλ��x
		int y;     // ��ʼλ��y
		int v;     // �Ƿ������ֵ0, 1
		int count; // ��������
	} l[MAX_PWL_LENGTH];

	struct {
		int v;
		int count;
	} max;
} ParseWordInfo;

class AutoReadImgWord
{
public:
	// ...
	AutoReadImgWord();

	// ...
	~AutoReadImgWord();

	// pixels=����ָ�� width=��� height=�߶�
	AutoReadImgWord(int* pixels, int width, int height, bool opengl, int byte = 4);

	// ����������Ϣ pixels=����ָ�� width=��� height=�߶�
	void SetPixels(int* pixels, int width, int height, bool opengl, int byte = 4);

	// ����ɫ
	AutoReadImgWord* SetBgColor(int color, int diff);

	// ������ɫ�������
	AutoReadImgWord* SetWordColor(int color, int diff);

	// ����������
	AutoReadImgWord* SetWordInfo(int min_w, int max_w, int min_h, int max_h);


	// ��ȡ������Ϣ
	int  GetPixel(int x, int y);

	// ��ȡ����
	int GetNum(int d_v = 0);

	// ��ȡ����
	int GetText(char* text, int* pnum = nullptr);

	// ��������Ϣ
	int FindWords();

	// �ָ��� ��ʱ���������һ��
	bool ExplodeWords(int si);

	// ��ӡ��
	void PrintWords();

	// �����Ƿ����Ҫ�� x=x y=y
	bool IsThePixel(int x, int y);
	// �����Ƿ����Ҫ�� pixel=ԭ����ԭɫ
	bool IsThePixel(int pixel);
protected:
	// ƥ������ ����-1ƥ�䲻�ɹ� 0-9��ȷ����
	int PregNum();
	// ��������0-���� 1-�� 2-����
	int ParseNum(int num, ParseWordInfo* p, int row_index, int col_index);
	// �Ƿ�Ϊ����0
	virtual int Is0(ParseWordInfo* p, int row_index, int col_index);
	// �Ƿ�Ϊ����0
	virtual int Is1(ParseWordInfo* p, int row_index, int col_index);
	// �Ƿ�Ϊ����0
	virtual int Is2(ParseWordInfo* p, int row_index, int col_index);
	// �Ƿ�Ϊ����0
	virtual int Is3(ParseWordInfo* p, int row_index, int col_index);
	// �Ƿ�Ϊ����0
	virtual int Is4(ParseWordInfo* p, int row_index, int col_index);
	// �Ƿ�Ϊ����0
	virtual int Is5(ParseWordInfo* p, int row_index, int col_index);
	// �Ƿ�Ϊ����0
	virtual int Is6(ParseWordInfo* p, int row_index, int col_index);
	// �Ƿ�Ϊ����0
	virtual int Is7(ParseWordInfo* p, int row_index, int col_index);
	// �Ƿ�Ϊ����0
	virtual int Is8(ParseWordInfo* p, int row_index, int col_index);
	// �Ƿ�Ϊ����0
	virtual int Is9(ParseWordInfo* p, int row_index, int col_index);
	// ParseWord
	void ParseWord();
	// ��ȡ���3��֮1��ȵķ���Ҫ����������
	int GetRowCountFromLeft3f1(int w3f1=0);
	// ��ȡ�ұ�3��֮1��ȵķ���Ҫ����������
	int GetRowCountFromRight3f1(int w3f1=0);
	// ��ȡһ��ƥ�������
	int  GetColCount(int x, int y, int length);
	// ��ȡһ�з��ϵ���Ŀ
	int  GetRowCount(int x, int y, int length);
protected:
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
	// ����Ϣ
	struct {
		int MinWidth;
		int MaxWidth;
		int MinHeight;
		int MaxHeiht;
	}   m_WordInfo;
private:
	struct m_BgColor{
		int l; // ����
		struct
		{
			int color; // ɫ
			int diff;  // ��
		} v[10]; // ֧��10
	} m_BgColor; // ������ɫ

	struct m_WordColor {
		int l; // ����
		struct
		{
			int color; // ɫ
			int diff;  // ��
		} v[10]; // ֧��10
	} m_WordColor; // ������ɫ

	// �ҵ���������
	int  m_nFindWords = 0;
	// ��ǰ��ȡ������
	int  m_nReadIndex = 0;
	// ����Ϣ
	struct {
		int StartX; // ��ʼλ��X
		int EndX;   // ����λ��X
		int StartY; // ��ʼλ��Y
		int EndY;   // ����λ��Y
		int Width;
		int Height;
	} m_WordInfos[MAX_WORD_LENGTH];
};