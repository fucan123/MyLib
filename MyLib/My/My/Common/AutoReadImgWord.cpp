#include "AutoReadImgWord.h"
#include <stdio.h>
#include <memory.h>

#define DIV2(v)   ((v>>1)+(v&0x01)) // ��2 �������1
#define ISP(x,y)  IsThePixel(x,y)
#define WD(n)     m_WordInfos[n]
#define CWD()     m_WordInfos[m_nReadIndex]
#define CWD_SX()  m_WordInfos[m_nReadIndex].StartX
#define CWD_EX()  m_WordInfos[m_nReadIndex].EndX
#define CWD_SY()  m_WordInfos[m_nReadIndex].StartY
#define CWD_EY()  m_WordInfos[m_nReadIndex].EndY
#define CWD_W()   m_WordInfos[m_nReadIndex].Width
#define CWD_H()   m_WordInfos[m_nReadIndex].Height
#define CWD_WD2() DIV2(m_WordInfos[m_nReadIndex].Width)
#define CWD_HD2() DIV2(m_WordInfos[m_nReadIndex].Height)


typedef struct _pt_
{
	int x;
	int y;
} PT;

// ...
AutoReadImgWord::AutoReadImgWord()
{
	m_pPixels = nullptr;
	memset(&m_BgColor, 0, sizeof(m_BgColor));
	memset(&m_WordColor, 0, sizeof(m_WordColor));
	memset(m_WordInfos, 0, sizeof(m_WordInfos));
}

// ...
AutoReadImgWord::~AutoReadImgWord()
{
	if (m_pPixels) {
		delete[] m_pPixels;
		m_pPixels = nullptr;
	}	
}

// pixels=����ָ�� width=��� height=�߶�
AutoReadImgWord::AutoReadImgWord(int* pixels, int width, int height, bool opengl, int byte)
{
	SetPixels(pixels, width, height, opengl, byte);
}

// ����������Ϣ pixels=����ָ�� width=��� height=�߶�
void AutoReadImgWord::SetPixels(int* pixels, int width, int height, bool opengl, int byte)
{
	if (m_pPixels) {
		if ((width * height) > (m_nMallocWidth * m_nMallocHeight)) { // ��ǰ�ڴ�Ĳ�����
			delete m_pPixels;
			m_pPixels = nullptr;
		}
	}
	if (!m_pPixels) {
		::printf("wh %d %d\n", width, height);
		m_pPixels = new int[width * height];
		m_nMallocWidth = width;
		m_nMallocHeight = height;
	}

	m_bOpengl = opengl;
	m_nWidth = width;
	m_nHeight = height;
	memcpy(m_pPixels, pixels, width * height * byte);

	//printf("SetPixels Ok\n");
}

// ������ɫ�������
AutoReadImgWord* AutoReadImgWord::SetBgColor(int color, int diff)
{
	m_BgColor.v[m_BgColor.l].color = color;
	m_BgColor.v[m_BgColor.l].diff = diff;
	m_BgColor.l++;

	return this;
}

// ������ɫ�������
AutoReadImgWord* AutoReadImgWord::SetWordColor(int color, int diff)
{
	m_WordColor.v[m_WordColor.l].color = color;
	m_WordColor.v[m_WordColor.l].diff = diff;
	m_WordColor.l++;

	return this;
}

// ����������
AutoReadImgWord* AutoReadImgWord::SetWordInfo(int min_w, int max_w, int min_h, int max_h)
{
	m_WordInfo = { min_w, max_w, min_h, max_h };
	return this;
}

// ��ȡ������Ϣ
int AutoReadImgWord::GetPixel(int x, int y)
{
	//printf("GetPixel:%d,%d\n", x, y);
	if (0 && m_bOpengl)
		return m_pPixels[(m_nHeight - y - 1) * m_nWidth + x];

	return m_pPixels[y * m_nWidth + x];
}

// ��ȡ����
int AutoReadImgWord::GetNum(int d_v)
{
	int num = d_v;
	char text[16];
	if (!GetText(text, &num))
		return d_v;

	//PrintWords(color, diff);
	//printf("����:%s ����:%d\n", text, num);
	return num;
}

// ��ȡ����
int AutoReadImgWord::GetText(char* text, int* pnum)
{
	m_nFindWords = 0;
	FindWords();

	int num = 0;
	int length = 0;
	for (m_nReadIndex = 0; m_nReadIndex < m_nFindWords; m_nReadIndex++) {
		int num = PregNum();
		if (num != -1) {
			text[length++] = num + '0';
		}
	}
	text[length] = 0;

	if (pnum)
		*pnum = num;

	::printf("GetText: %s\n", text);
	return length;
}

// ��������Ϣ
int AutoReadImgWord::FindWords()
{
	m_nFindWords = 0;

	int start_x = -1, end_x = -1, start_y = -1, end_y = -1;
	for (int x = 0; x < m_nWidth; x++) {
		int y_match_count = 0;
		for (int y = 0; y < m_nHeight; y++) {
			if (IsThePixel(GetPixel(x, y))) {
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
			printf("�ҵ���, ���:%d,%d λ��:%d-%d %d-%d\n", x-start_x, end_y - start_y + 1, start_x, x, start_y, end_y);

			if ((x - start_x) >= 2 && (end_y - start_y + 1) >= 6) {
				m_WordInfos[m_nFindWords].StartX = start_x;
				m_WordInfos[m_nFindWords].EndX = x - 1;
				m_WordInfos[m_nFindWords].StartY = start_y;
				m_WordInfos[m_nFindWords].EndY = end_y;
				m_WordInfos[m_nFindWords].Width = x - start_x;
				m_WordInfos[m_nFindWords].Height = end_y - start_y + 1;
				m_nFindWords++;

				while (ExplodeWords(m_nFindWords - 1));
			}

			start_x = -1, end_x = -1, start_y = -1, end_y = -1;
		}
	}

	return m_nFindWords;
}

// �ָ��� ��ʱ�������һ��
bool AutoReadImgWord::ExplodeWords(int si)
{
	if (!m_WordInfo.MinWidth || !m_WordInfo.MaxWidth)
		return false;

	for (m_nReadIndex = si; m_nReadIndex < m_nFindWords; m_nReadIndex++) {
		if (CWD_W() < m_WordInfo.MaxWidth) {
			continue;
		}
		
		int min = CWD_SX() + m_WordInfo.MinWidth;
		int max = CWD_SX() + m_WordInfo.MaxWidth - 1;
		for (int x = min; x < max; x++) {
			int count = 0; // ������������û������������
			for (int y = CWD_SY(); y <= CWD_EY(); y++) {
				if (!ISP(x, y) || !ISP(x + 1, y)) { // ���ڵ���һ��������
					count++;
				}
			}

			if (count == CWD_H()) { // ���������е��ڴ��ָ߶�, �ָ��
				::printf("<!--===�ָ�===-->\n");
				// ��һ��������
				m_WordInfos[m_nFindWords].StartX = x + 1;
				m_WordInfos[m_nFindWords].EndX = CWD_EX();
				m_WordInfos[m_nFindWords].StartY = CWD_SY();
				m_WordInfos[m_nFindWords].EndY = CWD_EY();
				m_WordInfos[m_nFindWords].Width = CWD_EX() - x;
				m_WordInfos[m_nFindWords].Height = CWD_EY() - CWD_SY() + 1;
				m_nFindWords++;

				// ��ǰ����ֵ
				CWD_EX() = x;
				CWD_W() = CWD_EX() - CWD_SX() + 1;

				return true;
			}
		}
		
	}

	return false;
}

// ��ӡ��
void AutoReadImgWord::PrintWords()
{
	for (int i = 0; i < m_nFindWords; i++) {
		m_nReadIndex = i;
		ParseWord();
		::printf("%d.[%d,%d][%d,%d](%d,%d)\n", i, m_WordInfos[i].StartX, m_WordInfos[i].StartY, m_WordInfos[i].EndX, m_WordInfos[i].EndY, m_WordInfos[i].Width, m_WordInfos[i].Height);
		for (int y = m_WordInfos[i].StartY; y <= m_WordInfos[i].EndY; y++) {
			for (int x = m_WordInfos[i].StartX; x <= m_WordInfos[i].EndX; x++) {
				if (IsThePixel(x, y)) {
					printf("1 ");
				}
				else {
					printf("0 ");
				}
			}
			printf("\n");
		}
		printf("\n\n");
	}
}

// �����Ƿ����Ҫ�� x=x y=y
bool AutoReadImgWord::IsThePixel(int x, int y)
{
	return IsThePixel(GetPixel(x, y));
}

// �����Ƿ����Ҫ�� pixel=ԭ����ԭɫ color=�Ƚϵ���ɫֵ diff=����Ĳ�ֵ
bool AutoReadImgWord::IsThePixel(int pixel)
{
	//printf("IsThePixel:%08X=%08X %08X\n", pixel, color, diff);
	if (m_BgColor.l > 0) {
		for (int l = 0; l < m_BgColor.l; l++) {
			bool result = false;
			if (m_BgColor.v[l].diff > 0) {
				int eq_c = 0;
				for (int i = 0; i < 3; i++) {
					int v = (pixel >> (i * 8)) & 0xff; // 09 05 03
					int c = (m_BgColor.v[l].color >> (i * 8)) & 0xff;
					int d = (m_BgColor.v[l].diff >> (i * 8)) & 0xff;
					if (v < c) {
						if ((v + d) >= c) {
							eq_c++;
						}
						
					}
					else if (v > c) {
						if ((c + d) >= v) {
							eq_c++;
						}
						
					}
					else {
						eq_c++;
					}
				}
				if (eq_c == 3) {
					result = true;
				}
				//::printf("eq c:%d\n", eq_c);
			}
			else {
				result = (pixel & 0x00ffffff) == (m_BgColor.v[l].color & 0x00ffffff);
			}
			if (result) {
				//::printf("�Ǳ���ɫ %08x == %08X\n", m_BgColor.v[l].color, pixel);
				return false;
			}
		}
		return true;
	}
	else if (m_WordColor.l > 0) {
		for (int l = 0; l < m_WordColor.l; l++) {
			bool result = false;
			if (m_WordColor.v[l].diff > 0) {
				int eq_c = 0;
				for (int i = 0; i < 3; i++) {
					int v = (pixel >> (i * 8)) & 0xff; // 09 05 03
					int c = (m_WordColor.v[l].color >> (i * 8)) & 0xff;
					int d = (m_WordColor.v[l].diff >> (i * 8)) & 0xff;
					if (v < c) {
						if ((v + d) >= c) {
							eq_c++;
						}

					}
					else if (v > c) {
						if ((c + d) >= v) {
							eq_c++;
						}

					}
					else {
						eq_c++;
					}
				}
				if (eq_c == 3) {
					result = true;
				}
			}
			else {
				result = (pixel & 0x00ffffff) == (m_WordColor.v[l].color & 0x00ffffff);
			}

			if (result) {
				return true;
			}
		}
		return false;
	}

	return false;
}

// ƥ������ index=m_WordInfos���� ����-1ƥ�䲻�ɹ� 0-9��ȷ����
int AutoReadImgWord::PregNum()
{
	if (CWD_W() < 5) // ���û�дﵽ5
		return -1;
	if (m_WordInfo.MaxWidth && CWD_W() >= m_WordInfo.MaxWidth)
	if (CWD_H() < 6) // �߶�û�дﵽ6
		return -1;

	//ParseWord();

#if 0
	if (Is0())
		return 0;
	if (Is1())
		return 1;
	if (Is2())
		return 2;
	if (Is3())
		return 3;
	if (Is4())
		return 4;
	if (Is5())
		return 5;
	if (Is6())
		return 6;
	if (Is7())
		return 7;
	if (Is8())
		return 8;
	if (Is9())
		return 9;
#endif

	return -1;
}

// �������� 0-���� 1-�� 2-����
int AutoReadImgWord::ParseNum(int num, ParseWordInfo* p, int row_index, int col_index)
{
	switch (num)
	{
	case 0:
		return Is0(p, row_index, col_index);
	case 1:
		return Is1(p, row_index, col_index);
	case 2:
		return Is2(p, row_index, col_index);
	case 3:
		return Is3(p, row_index, col_index);
	case 4:
		return Is4(p, row_index, col_index);
	case 5:
		return Is5(p, row_index, col_index);
	case 6:
		return Is6(p, row_index, col_index);
	case 7:
		return Is7(p, row_index, col_index);
	case 8:
		return Is8(p, row_index, col_index);
	case 9:
		return Is9(p, row_index, col_index);
	default:
		return 0;
	}

	return 0;
}

// �Ƿ�Ϊ����0
int AutoReadImgWord::Is0(ParseWordInfo* p, int row_index, int col_index)
{
	if (row_index > -1) {
		if (p->max.count == CWD_W())
			return 0;

		return 1;
	}
	else if (col_index > -1) {

	}

	return 0;
}

// �Ƿ�Ϊ����1
int AutoReadImgWord::Is1(ParseWordInfo* p, int row_index, int col_index)
{
	return 0;
}

// �Ƿ�Ϊ����2
int AutoReadImgWord::Is2(ParseWordInfo* p, int row_index, int col_index)
{
	return 0;
}

// �Ƿ�Ϊ����3
int AutoReadImgWord::Is3(ParseWordInfo* p, int row_index, int col_index)
{
	return 0;
}

// �Ƿ�Ϊ����4
int AutoReadImgWord::Is4(ParseWordInfo* p, int row_index, int col_index)
{
	return 0;
}

// �Ƿ�Ϊ����5
int AutoReadImgWord::Is5(ParseWordInfo* p, int row_index, int col_index)
{
	return 0;
}

// �Ƿ�Ϊ����6
int AutoReadImgWord::Is6(ParseWordInfo* p, int row_index, int col_index)
{
	return 0;
}

// �Ƿ�Ϊ����7
int AutoReadImgWord::Is7(ParseWordInfo* p, int row_index, int col_index)
{
	return 0;
}

// �Ƿ�Ϊ����8
int AutoReadImgWord::Is8(ParseWordInfo* p, int row_index, int col_index)
{
	return 0;
}


// �Ƿ�Ϊ����9
int AutoReadImgWord::Is9(ParseWordInfo* p, int row_index, int col_index)
{
	return 0;
}

// ParseWord
void AutoReadImgWord::ParseWord()
{
	int preg_num = 10;
	int values[10] = { 0 }; // ƥ�䵽������, ���Ϊ0��ʾ, û��ƥ�䵽, 1ƥ�䵽, 2����ƥ�䵽
	memset(values, 1, sizeof(values));
	ParseWordInfo data;  // ����������
	for (int y = CWD_SY(), n = 0; y <= CWD_EY(); y++, n++) {
		if (preg_num <= 0)
			break;

		data.ln = 0;
		data.max = { -1, 0 };
		int v = -1;
		int c = 0;
		for (int x = CWD_SX(); x <= CWD_EX(); x++) {
			int r = ISP(x, y) ? 1 : 0;
			if (v == r || v == -1) {
					
				c++;	
			}
			else {
				//::printf("%d����%d\n", v, c);
				data.l[data.ln] = { x, y, v, c };
				if (c > data.max.count) {
					data.max = { v, c };
				}
				if (++data.ln >= MAX_PWL_LENGTH)
					break;

				c = 1;
			}

			v = r;
			if (x == CWD_EX()) {
				//::printf("%d����%d xxx\n", v, c);
				data.l[data.ln] = { x, y, v, c };
				if (c > data.max.count) {
					data.max = { v, c };
				}
				if (++data.ln >= MAX_PWL_LENGTH)
					break;
			}
		}

		::printf("%d ����: %d/%d %d\n", n, data.max.count, CWD_W(), data.max.v);
		if (data.max.count == CWD_W()) {
			::printf("����\n");
		}
		for (int i = 0; i < 10; i++) {
			if (values[i]) {
				int result = ParseNum(i, &data, n, -1);
				if (result == 0) {
					values[i] = 0;
					preg_num--;
					::printf("preg num:%d\n", preg_num);
				}
			}
		}
	}
}

// ��ȡ���3��֮1��ȵķ���Ҫ����������
int AutoReadImgWord::GetRowCountFromLeft3f1(int w3f1)
{
	int count = 0;
	w3f1 = w3f1 ? w3f1 : CWD_W() / 3;
	for (int x = 0; x < w3f1; x++) {
		int num = GetRowCount(x + CWD_SX(), CWD_SY(), CWD_H());
		//::printf("%d GetRowCountFrom3f1 %d %d %d (%d)\n", m_nReadIndex, x + CWD_SX(), CWD_SY(), CWD_H(), num);
		if (num > count)
			count = num;
	}
	return count;
}

// ��ȡ�ұ�3��֮1��ȵķ���Ҫ����������
int AutoReadImgWord::GetRowCountFromRight3f1(int w3f1)
{
	int count = 0;
	w3f1 = w3f1 ? w3f1 : CWD_W() / 3;
	for (int x = CWD_EX() - w3f1 + 1 ; x <= CWD_EX(); x++) {
		int num = GetRowCount(x, CWD_SY(), CWD_H());
		//::printf("%d GetRowCountFrom3f1 %d %d %d (%d)\n", m_nReadIndex, x + CWD_SX(), CWD_SY(), CWD_H(), num);
		if (num > count)
			count = num;
	}
	return count;
}

// ��ȡһ��ƥ�������
int AutoReadImgWord::GetColCount(int x, int y, int length)
{
	int count = 0;
	for (int i = 0; i < length; i++) {
		if (ISP(x + i, y))
			count++;
	}
	return count;
}

// ��ȡһ�з��ϵ���Ŀ
int AutoReadImgWord::GetRowCount(int x, int y, int length)
{
	int count = 0;
	for (int i = 0; i < length; i++) {
		//::printf("%08X \n", GetPixel(x, y + i));
		if (ISP(x, y + i)) {
			//::printf("IsThePixel\n");
			count++;
		}
			
	}
	return count;
}
