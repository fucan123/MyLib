#include "HtmlUI.h"
#include <stdio.h>

// 创建
void HtmlUI::Create(const wchar_t* dllPath, HWND hWnd, int width, int height)
{
	::wkeSetWkeDllPath(dllPath);
	::wkeInitialize();
	m_Web = ::wkeCreateWebWindow(WKE_WINDOW_TYPE_CONTROL, hWnd, 0, 0, width, height);
}

// 加载(先创建后加载) string=本地地址或者URL file=是否本地地址或URL
void HtmlUI::Load(const char* string, bool file)
{
	if (file)
		::wkeLoadFile(m_Web, string);
	else
		::wkeLoadHTML(m_Web, string);

	::wkeShowWindow(m_Web, TRUE);
}

// 加载(先创建后加载) string=本地地址或者URL file=是否本地地址或URL
void HtmlUI::Load(const wchar_t* string, bool file)
{
	if (file)
		::wkeLoadFileW(m_Web, string);
	else
		::wkeLoadHTMLW(m_Web, string);

	::wkeShowWindow(m_Web, TRUE);
}

// 设置文档加载完回调函数
void HtmlUI::SetDocumentReadyCallback(HtmlDocumentReadyCallback func, void* param)
{
	::wkeOnDocumentReady(m_Web, func, param);
}

//  设置JavaScript调用回调
void HtmlUI::SetJsCall()
{
	::jsBindFunction("CallCpp", js_Func, 1);
}

// 调用JavaScript
void HtmlUI::CallJs(JsData* pJsData)
{
	wchar_t text[256];
	int size = MultiByteToWideChar(CP_ACP, 0, pJsData->Text, -1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, pJsData->Text, -1, text, size<256?size:255);

	jsExecState es = ::wkeGlobalExec(m_Web);
	jsValue f = ::jsGetGlobal(es, "CallJs");
	jsValue v[1];
	v[0] = ::jsEmptyObject(es);
	::jsSet(es, v[0], "Name", ::jsString(es, pJsData->Name));
	::jsSet(es, v[0], "Id", ::jsString(es, pJsData->Id));
	::jsSet(es, v[0], "Text", ::jsStringW(es, text));

	jsValue arr = ::jsEmptyArray(es);
	for (int i = 0; i < sizeof(pJsData->Value) / sizeof(int); i++) {
		::jsSetAt(es, arr, i, jsInt(pJsData->Value[i]));
	}
	::jsSet(es, v[0], "Value", arr);

	::jsCallGlobal(es, f, v, 1);
}

// 获取WebView
wkeWebView HtmlUI::GetWebView()
{
	return m_Web;
}

//  JavaScript调用回调
jsValue JS_CALL HtmlUI::js_Func(jsExecState es)
{
	jsValue arg = ::jsArg(es, 0);
	jsValue jvName = ::jsGet(es, arg, "name");
	jsValue id = ::jsGet(es, arg, "id");
	jsValue value = ::jsGet(es, arg, "value");
	if (jsIsJsValueValid(es, id)) {
		::printf("id: %d %s\n", ::jsToInt(es, id), ::jsToString(es, id));
	}
	::printf("JS_CALL %s.\n", ::jsToString(es, jvName));
	::printf("value:");

	for (int i = 0; i < 10; i++) {
		::printf("%d ", ::jsToInt(es, jsGetAt(es, value, i)));
	}
	::printf("\n");
	return ::jsInt(168);
}
