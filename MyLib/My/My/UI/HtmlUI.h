#pragma once
#include "wke.h"
#include "HtmlUIData.h"
#include <Windows.h>

typedef void (WKE_CALL_TYPE* HtmlDocumentReadyCallback)(wkeWebView webView, void* param);

class HtmlUI
{
public:
protected:
	wkeWebView m_Web;
private:

public:
	// ����
	void Create(const wchar_t* dllPath, HWND hWnd, int width, int height);
	// ����(�ȴ��������) string=���ص�ַ����URL file=�Ƿ񱾵ص�ַ��URL
	void Load(const char* string, bool file=true);
	// ����(�ȴ��������) string=���ص�ַ����URL file=�Ƿ񱾵ص�ַ��URL
	void Load(const wchar_t* string, bool file=true);
	// �����ĵ�������ص�����
	void SetDocumentReadyCallback(HtmlDocumentReadyCallback func, void* param);
	// ����JavaScript���ûص�
	void SetJsCall();
	// ����JavaScript
	void CallJs(JsData* pJsData);
	// ��ȡWebView
	wkeWebView GetWebView();
public:
	// JavaScript���ûص�
	static jsValue JS_CALL js_Func(jsExecState es);
};