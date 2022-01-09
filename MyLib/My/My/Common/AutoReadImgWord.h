#pragma once

// 最多支持shuzi
#define MAX_WORD_LENGTH 10
#define MAX_PWL_LENGTH  8

typedef struct __parse_word_info__
{
	int ln; // 列表数量
	struct {
		int x;     // 开始位置x
		int y;     // 开始位置y
		int v;     // 是否符合数值0, 1
		int count; // 连续数量
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

	// pixels=像数指针 width=宽度 height=高度
	AutoReadImgWord(int* pixels, int width, int height, bool opengl, int byte = 4);

	// 设置像数信息 pixels=像数指针 width=宽度 height=高度
	void SetPixels(int* pixels, int width, int height, bool opengl, int byte = 4);

	// 背景色
	AutoReadImgWord* SetBgColor(int color, int diff);

	// 设置颜色与差异数
	AutoReadImgWord* SetWordColor(int color, int diff);

	// 设置字数据
	AutoReadImgWord* SetWordInfo(int min_w, int max_w, int min_h, int max_h);


	// 获取像数信息
	int  GetPixel(int x, int y);

	// 获取数字
	int GetNum(int d_v = 0);

	// 获取文字
	int GetText(char* text, int* pnum = nullptr);

	// 查找字信息
	int FindWords();

	// 分割字 有时多个会连在一起
	bool ExplodeWords(int si);

	// 打印字
	void PrintWords();

	// 像数是否符合要求 x=x y=y
	bool IsThePixel(int x, int y);
	// 像数是否符合要求 pixel=原像数原色
	bool IsThePixel(int pixel);
protected:
	// 匹配数字 返回-1匹配不成功 0-9正确数字
	int PregNum();
	// 分析数字0-不是 1-是 2-或许
	int ParseNum(int num, ParseWordInfo* p, int row_index, int col_index);
	// 是否为数字0
	virtual int Is0(ParseWordInfo* p, int row_index, int col_index);
	// 是否为数字0
	virtual int Is1(ParseWordInfo* p, int row_index, int col_index);
	// 是否为数字0
	virtual int Is2(ParseWordInfo* p, int row_index, int col_index);
	// 是否为数字0
	virtual int Is3(ParseWordInfo* p, int row_index, int col_index);
	// 是否为数字0
	virtual int Is4(ParseWordInfo* p, int row_index, int col_index);
	// 是否为数字0
	virtual int Is5(ParseWordInfo* p, int row_index, int col_index);
	// 是否为数字0
	virtual int Is6(ParseWordInfo* p, int row_index, int col_index);
	// 是否为数字0
	virtual int Is7(ParseWordInfo* p, int row_index, int col_index);
	// 是否为数字0
	virtual int Is8(ParseWordInfo* p, int row_index, int col_index);
	// 是否为数字0
	virtual int Is9(ParseWordInfo* p, int row_index, int col_index);
	// ParseWord
	void ParseWord();
	// 获取左边3分之1宽度的符合要求的最大数量
	int GetRowCountFromLeft3f1(int w3f1=0);
	// 获取右边3分之1宽度的符合要求的最大数量
	int GetRowCountFromRight3f1(int w3f1=0);
	// 获取一行匹配的数量
	int  GetColCount(int x, int y, int length);
	// 获取一列符合的数目
	int  GetRowCount(int x, int y, int length);
protected:
	// 是否opengl 此为倒置像数
	bool m_bOpengl = false;
	// 保存像数信息数组
	int* m_pPixels = nullptr;
	// 宽度
	int m_nWidth = 0;
	// 高度
	int m_nHeight = 0;
	// 内存分配宽度
	int m_nMallocWidth = 0;
	// 内存分配高度
	int m_nMallocHeight = 0;
	// 字信息
	struct {
		int MinWidth;
		int MaxWidth;
		int MinHeight;
		int MaxHeiht;
	}   m_WordInfo;
private:
	struct m_BgColor{
		int l; // 长度
		struct
		{
			int color; // 色
			int diff;  // 差
		} v[10]; // 支持10
	} m_BgColor; // 字体颜色

	struct m_WordColor {
		int l; // 长度
		struct
		{
			int color; // 色
			int diff;  // 差
		} v[10]; // 支持10
	} m_WordColor; // 字体颜色

	// 找到的字数量
	int  m_nFindWords = 0;
	// 当前读取的索引
	int  m_nReadIndex = 0;
	// 字信息
	struct {
		int StartX; // 开始位置X
		int EndX;   // 结束位置X
		int StartY; // 开始位置Y
		int EndY;   // 结束位置Y
		int Width;
		int Height;
	} m_WordInfos[MAX_WORD_LENGTH];
};