#pragma once

#define HTTP_POST 0
#define HTTP_GET  1
#define HTTP_PUT  2

typedef unsigned int HTTP_STATUS;

class HttpSocket
{
public:
	// ���͵����ݻ򷵻صĽ��, ��Ҫ������ռ�,����ʱ�����ͷ����POST����
	char* m_chData;
	// ���ص���������
	char* m_chBody;
private:
	// ���͵����ݴ�С
	int m_nSendDataCount;
	// ��ǰPOST���ݴ�С
	int m_nPostCount;
	// ���ؽ����С, ����header
	int m_nResponseCount;
	// �����������ݴ�С
	int m_nBodyCount;
	// m_chData Size Max
	int m_nDataMaxSize;
public:
    // data_size=m_chData���ͻ򷵻ص����ݰ���ͷ����С, ����ʵ��ȥ����
	HttpSocket(int data_size=4096);
	// ����
	~HttpSocket();
	// ����
	void Release();
	// GET
	HTTP_STATUS Get(const char* host, const char* path, int port=80);
	// POST
	HTTP_STATUS Post(const char* host, const char* path, int port=80);
	// PUT
	HTTP_STATUS Put(const char* host, const char* path, int port=80);
	// ����
	HTTP_STATUS Request(const char* host, const char* path, int port=80, int type=HTTP_GET);
	// ��ȡ��������
	const char* GetResponse();
	// ��ȡ���س���
	int GetResponseCount();
	// ��ȡ������������
	const char* GetBody();
	// ��ȡ�������峤��
	int GetBodyCount();
	// ��ȡ����ͷ����Ϣ ����ֵ=���ݵĳ���
	int GetResponseHeader(const char* key, char* save, int save_len);
private:
	// �����Ӧͷ��״̬
	HTTP_STATUS GetResponseStatus();
	// ������������
	void FormatResponse();
public:
	// ���ͷ������
	bool AddHeader(const char* header);
	// ���ͷ������
	void AddHeader(const char* key, const char* value);
	// ���ͷ������
	void AddHeader(const char* key, int value);
	// ���Cookie
	void AddCookie(const char* key, const char* value);
	// ���Cookie
	void AddCookie(const char* key, int value);
	// ���POST����
	void AddPost(const char* key, const char* value);
	// ���POST����
	void AddPost(const char* key, int value);
	
public:
	// �Ƿ�ת��GB2312
	bool m_GB2312 = false;
};