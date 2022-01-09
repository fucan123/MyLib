#pragma once

#define HTTP_POST 0
#define HTTP_GET  1
#define HTTP_PUT  2

typedef unsigned int HTTP_STATUS;

class HttpSocket
{
public:
	// 发送的数据或返回的结果, 需要先申请空间,发送时会存入头部和POST数据
	char* m_chData;
	// 返回的主体内容
	char* m_chBody;
private:
	// 发送的数据大小
	int m_nSendDataCount;
	// 当前POST数据大小
	int m_nPostCount;
	// 返回结果大小, 包括header
	int m_nResponseCount;
	// 返回主题内容大小
	int m_nBodyCount;
	// m_chData Size Max
	int m_nDataMaxSize;
public:
    // data_size=m_chData发送或返回的内容包括头部大小, 根据实际去申请
	HttpSocket(int data_size=4096);
	// 析构
	~HttpSocket();
	// 清理
	void Release();
	// GET
	HTTP_STATUS Get(const char* host, const char* path, int port=80);
	// POST
	HTTP_STATUS Post(const char* host, const char* path, int port=80);
	// PUT
	HTTP_STATUS Put(const char* host, const char* path, int port=80);
	// 请求
	HTTP_STATUS Request(const char* host, const char* path, int port=80, int type=HTTP_GET);
	// 获取返回数据
	const char* GetResponse();
	// 获取返回长度
	int GetResponseCount();
	// 获取返回主体数据
	const char* GetBody();
	// 获取返回主体长度
	int GetBodyCount();
	// 获取返回头部信息 返回值=内容的长度
	int GetResponseHeader(const char* key, char* save, int save_len);
private:
	// 获得响应头部状态
	HTTP_STATUS GetResponseStatus();
	// 解析返回内容
	void FormatResponse();
public:
	// 添加头部数据
	bool AddHeader(const char* header);
	// 添加头部数据
	void AddHeader(const char* key, const char* value);
	// 添加头部数据
	void AddHeader(const char* key, int value);
	// 添加Cookie
	void AddCookie(const char* key, const char* value);
	// 添加Cookie
	void AddCookie(const char* key, int value);
	// 添加POST数据
	void AddPost(const char* key, const char* value);
	// 添加POST数据
	void AddPost(const char* key, int value);
	
public:
	// 是否转成GB2312
	bool m_GB2312 = false;
};