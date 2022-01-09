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
	// 创建
	void Create(const wchar_t* dllPath, HWND hWnd, int width, int height);
	// 加载(先创建后加载) string=本地地址或者URL file=是否本地地址或URL
	void Load(const char* string, bool file=true);
	// 加载(先创建后加载) string=本地地址或者URL file=是否本地地址或URL
	void Load(const wchar_t* string, bool file=true);
	// 设置文档加载完回调函数
	void SetDocumentReadyCallback(HtmlDocumentReadyCallback func, void* param);
	// 设置JavaScript调用回调
	void SetJsCall();
	// 调用JavaScript
	void CallJs(JsData* pJsData);
	// 获取WebView
	wkeWebView GetWebView();
public:
	// JavaScript调用回调
	static jsValue JS_CALL js_Func(jsExecState es);
};