#pragma once

#include <afxinet.h>

#define HTTP_POST 0
#define HTTP_GET  1

#define HOST_NAME L"htx.myhostcpp.com"
#define HOST_PORT 80

typedef DWORD HTTP_STATUS;

class HttpClient
{
public:
	HttpClient() { }
	HTTP_STATUS Request(WCHAR* host, WCHAR* path, CString& result, int type=HTTP_POST);
	void SetCookie(char* cookie);
	void SetCookie(wchar_t* cookie);
	void AddParam(WCHAR* key, CHAR* value);
	void AddParam(WCHAR* key, WCHAR* value);
	void AddParam(WCHAR* key, CString value);
	void ResetParam();
public:
protected:
	CString m_Cookie;
	CString m_SendParam;
	CInternetSession* m_pSession;
	CHttpFile* m_pHttpFile;
};