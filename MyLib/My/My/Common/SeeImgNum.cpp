#include "SeeImgNum.h"
#include <stdio.h>
#include <memory.h>

// ...
SeeImgNum::SeeImgNum()
{
	m_pPixels = nullptr;
}

// ...
SeeImgNum::~SeeImgNum()
{
	if (m_pPixels)
		delete m_pPixels;
	m_pPixels = nullptr;
}

// pixels=像数指针 width=宽度 height=高度
SeeImgNum::SeeImgNum(int * pixels, int width, int height, bool opengl, int byte)
{
	SetPixels(pixels, width, height, opengl, byte);
}

// 设置像数信息 pixels=像数指针 width=宽度 height=高度
void SeeImgNum::SetPixels(int* pixels, int width, int height, bool opengl, int byte)
{
	if (m_pPixels) {
		if ((width * height) > (m_nMallocWidth * m_nMallocHeight)) { // 以前内存的不够了
			delete m_pPixels;
			m_pPixels = nullptr;
		}
	}
	if (!m_pPixels) {
		m_pPixels = new int[width * height];
		m_nMallocWidth = width;
		m_nMallocHeight = height;
	}

	m_bOpengl = opengl;
	m_nWidth = width;
	m_nHeight = height;
	memcpy(m_pPixels, pixels, width*height*byte);

	//printf("SetPixels Ok\n");
}

// 设置字模 models=字模数组 width=字模宽 height=字模高
void SeeImgNum::SetModels(int* models, int width, int height)
{
	for (int i = 0; i <= 9; i++) {
		m_Models[i] = &models[i * width * height];
	}

	SetModelWidthHeight(width, height);
}

// 设置字模 model=字模 num=字模对应数值
void SeeImgNum::SetModel(int* model, int num)
{
	if (num >= 0 || num <= 9)
		m_Models[num] = model;
}

// 设置字模宽高 width=字模宽 height=字模高
void SeeImgNum::SetModelWidthHeight(int width, int height)
{
	m_nModelWidth = width;
	m_nModelHeight = height;
}

// 获取像数信息
int SeeImgNum::GetPixel(int x, int y)
{
	//printf("GetPixel:%d,%d\n", x, y);
	if (0 && m_bOpengl)
		return m_pPixels[(m_nHeight-y-1)* m_nWidth + x];

	return m_pPixels[y * m_nWidth + x];
}

// 获取数字
int SeeImgNum::GetNum(int color, int diff, int d_v)
{
	int num = d_v;
	char text[16];
	if (!GetText(text, color, diff, &num))
		return d_v;

	//PrintWords(color, diff);
	//printf("文字:%s 数字:%d\n", text, num);
	return num;
}

// 获取文字
int SeeImgNum::GetText(char* text, int color, int diff, int* pnum)
{
	m_nFindWords = 0;
	FindWords(color, diff);

	int num = 0;
	int length = 0;
	int model_pixel_count = m_nModelWidth * m_nModelHeight;
	int best_count = model_pixel_count - (model_pixel_count / 10);
	for (int i = 0; i < m_nFindWords; i++) {
		int match_word = 0, match_count = 0;
		for (int num = 0; num <= 9; num++) {
			int count = PregModels(i, num, color, diff);
			//printf("%d匹配数量:%d\n", num, count);
			if (count > match_count) {
				match_word = num;
				match_count = count;
				if (match_count == model_pixel_count)
					break;
			}
		}
		if (match_count > best_count) {
			num = num * 10 + match_word;
			char ch = match_word + '0';
			//printf("best_count:%d ch:%c\n", best_count, ch);
			text[length++] = ch;
		}
		//printf("\n-----------------------\n");
	}
	text[length] = 0;

	if (pnum)
		*pnum = num;

	return length;
}

// 匹配字模, 返回匹配数量
int SeeImgNum::PregModels(int word_index, int model_index, int color, int diff)
{
	if (word_index >= m_nFindWords)
		return 0;

	int* models = GetModels(model_index);
	if (models == nullptr)
		return 0;

	int preg_width = m_WordInfos[word_index].EndX - m_WordInfos[word_index].StartX + 1;
	int preg_height = m_WordInfos[word_index].EndY - m_WordInfos[word_index].StartY + 1;
	if (preg_width > m_nModelWidth)
		preg_width = m_nModelWidth;
	if (preg_height > m_nModelHeight)
		preg_height = m_nModelHeight;

	int no_count = 0;
	for (int y = 0; y < preg_height; y++) {
		for (int x = 0; x < preg_width; x++) {
			int pixel = GetPixel(m_WordInfos[word_index].StartX + x, m_WordInfos[word_index].StartY + y);
			int v = IsThePixel(pixel, color, diff) ? 1 : 0;
			if (v != models[y * m_nModelWidth + x]) { // 不匹配
				no_count++;
			}
		}
	}
	//printf("%d:preg_width:%d preg_height:%d 不匹配:%d\n", model_index, preg_width, preg_height, no_count);
	return (m_nModelWidth * m_nModelHeight) - no_count;
}

// 获取字模数组
int* SeeImgNum::GetModels(int index)
{
	return index >= 0 && index <= 9 ? m_Models[index] : nullptr;
}

// 查找字信息
int SeeImgNum::FindWords(int color, int diff)
{
	m_nFindWords = 0;

	int start_x = -1, end_x = -1, start_y = -1, end_y = -1;
	for (int x = 0; x < m_nWidth; x++) {
		int y_match_count = 0;
		for (int y = 0; y < m_nHeight; y++) {
			if (IsThePixel(GetPixel(x, y), color, diff)) {
				if (start_x == -1) {
					start_x = x;
				}

				if (start_y == -1) {
					start_y = y;
				}
				else if (start_y > y) {
					start_y = y;
				}

				if (end_y == -1) {
					end_y = y;
				}
				else if (end_y < y) {
					end_y = y;
				}

				y_match_count++;
			}
		}

		if (start_x != -1 && (y_match_count == 0 || (x + 1) == m_nWidth)) {
			//printf("找到字, 宽高:%d,%d 位置:%d-%d %d-%d\n", x-start_x, end_y - start_y + 1, start_x, x, start_y, end_y);

			if ((x - start_x) >= 2 && (end_y - start_y + 1) >= m_nModelHeight - 2) {
				m_WordInfos[m_nFindWords].StartX = start_x;
				m_WordInfos[m_nFindWords].EndX = x - 1;
				m_WordInfos[m_nFindWords].StartY = start_y;
				m_WordInfos[m_nFindWords].EndY = end_y;
				m_nFindWords++;
			}

			start_x = -1, end_x = -1, start_y = -1, end_y = -1;
		}
	}

	return m_nFindWords;
}

// 打印字
void SeeImgNum::PrintWords(int color, int diff)
{
	for (int i = 0; i < m_nFindWords; i++) {
		for (int y = m_WordInfos[i].StartY; y <= m_WordInfos[i].EndY; y++) {
			for (int x = m_WordInfos[i].StartX; x <= m_WordInfos[i].EndX; x++) {
				if (IsThePixel(GetPixel(x, y), color, diff)) {
					printf("1 ");
				}
				else {
					printf(" ");
				}
			}
			printf("\n");
		}
		printf("\n\n");
	}
}

// 像数是否符合要求 pixel=原像数原色 color=比较的颜色值 diff=允许的差值
bool SeeImgNum::IsThePixel(int pixel, int color, int diff)
{
	//printf("IsThePixel:%08X=%08X %08X\n", pixel, color, diff);
	if (diff > 0) {
		for (int i = 0; i < 3; i++) {
			int v = (pixel >> (i * 8)) & 0xff;
			int c = (color >> (i * 8)) & 0xff;
			int d = (diff >> (i * 8)) & 0xff;
			if (v < (c - d) || v > (c + d)) { // 不符合要求
				return false;
			}
		}
		return true;
	}

	return (pixel&0x00ffffff) == (color&0x00ffffff);
}
