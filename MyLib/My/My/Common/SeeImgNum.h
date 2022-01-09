#pragma once

// 最多支持shuzi
#define MAX_WORD_LENGTH 10

class SeeImgNum
{
public:
	// ...
	SeeImgNum();

	// ...
	~SeeImgNum();

	// pixels=像数指针 width=宽度 height=高度
	SeeImgNum(int* pixels, int width, int height, bool opengl, int byte = 4);

	// 设置像数信息 pixels=像数指针 width=宽度 height=高度
	void SetPixels(int* pixels, int width, int height, bool opengl, int byte=4);

	// 设置字模 models=字模数组 width=字模宽 height=字模高
	void SetModels(int* models, int width, int height);

	// 设置字模 model=字模 num=字模对应数值
	void SetModel(int* model, int num);

	// 设置字模宽高 width=字模宽 height=字模高
	void SetModelWidthHeight(int width, int height);

	// 获取像数信息
	int  GetPixel(int x, int y);

	// 获取数字
	int GetNum(int color, int diff, int d_v=0);

	// 获取文字
	int GetText(char* text, int color, int diff, int* pnum=nullptr);

	// 查找字信息
	int FindWords(int color, int diff);

	// 打印字
	void PrintWords(int color, int diff);

	// 像数是否符合要求 pixel=原像数原色 color=比较的颜色值 diff=允许的差值
	bool IsThePixel(int pixel, int color, int diff);
private:
	// 匹配字模, 返回匹配数量
	int PregModels(int word_index, int model_index, int color, int diff);
	// 获取字模数组
	int* GetModels(int index);
private:
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
private:
	// 字模
	int* m_Models[10];
	// 找到的字数量
	int  m_nFindWords = 0;
	// 字模宽度
	int  m_nModelWidth = 0;
	// 字模高度
	int  m_nModelHeight = 0;
	// 字信息
	struct {
		int StartX; // 开始位置X
		int EndX;   // 结束位置X
		int StartY; // 开始位置Y
		int EndY;   // 结束位置Y
	} m_WordInfos[MAX_WORD_LENGTH];
};