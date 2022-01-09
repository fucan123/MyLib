#include "HttpClient.h"

HTTP_STATUS HttpClient::Request(WCHAR* host, WCHAR* path, CString& result, int type)
{
	result.Empty();

	HTTP_STATUS status = 0;
	CInternetSession session(_T("mm160"));
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 15000);
	try {
		//MessageBox(NULL, m_SendParam, L"发送的参数", MB_OK);
		CHttpConnection* pServer = session.GetHttpConnection(host, (INTERNET_PORT)HOST_PORT);
		CHttpFile* pFile = pServer->OpenRequest(type, path, NULL, 1, NULL, NULL, INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_COOKIES);
		if (type == HTTP_POST) {
			pFile->AddRequestHeaders(L"Content-Type: application/x-www-form-urlencoded");
		}
		
		if (!m_Cookie.IsEmpty()) {
			CString cookie = L"Cookie: JSESSIONID=";
			cookie += m_Cookie;
			pFile->AddRequestHeaders(cookie);
		}
		
		USES_CONVERSION;
		char* param = T2A(m_SendParam.GetBuffer(0));
		// 开始发送请求
		pFile->SendRequest(NULL, 0, param, strlen(param));
		pFile->QueryInfoStatusCode(status);
		printf("HTTP CODE: %d\n", status);
		if (status == HTTP_STATUS_OK) {
			CString newline;
			//printf("newline:%ws\n", newline);
			while (pFile->ReadString(newline)) { // 循环读取每行内容 
				result += newline + "\r\n";
			}
			//printf("内容:%s\n", result.GetBuffer()); // 显示返回内容 
		}

		if (pFile)
			delete pFile;
		if (pServer)
			delete pServer;
	}
	catch (CInternetException* pEx) {
		printf("错误:服务器未响应！！！\n"); // 显示异常信息 
		status = 0;
	}
	session.Close();
	return status;
}

void HttpClient::SetCookie(char * cookie)
{
	m_Cookie = cookie;
}

void HttpClient::SetCookie(wchar_t * cookie)
{
	m_Cookie = cookie;
}

void HttpClient::AddParam(WCHAR* key, CHAR* value)
{
	m_SendParam += key;
	m_SendParam += L"=";
	m_SendParam += value;
	m_SendParam += L"&";
}

void HttpClient::AddParam(WCHAR* key, WCHAR* value)
{
	m_SendParam += key;
	m_SendParam += L"=";
	m_SendParam += value;
	m_SendParam += L"&";
}

void HttpClient::AddParam(WCHAR* key, CString value)
{
	m_SendParam += key;
	m_SendParam += L"=";
	m_SendParam += value;
	m_SendParam += L"&";
}

void HttpClient::ResetParam()
{
	m_SendParam.Empty();
}
