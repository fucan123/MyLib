#pragma once
#define MAX_FILE_CONF_KL 64    // ����ֵ����
#define MAX_FILE_CONF_VC 64    // ���ֵ����
#define MAX_FILE_CONF_VL 256   // ���ÿ��ֵ����

// ��ȡ�����ļ���־
enum class FileConfigReadFlag
{
	FileConfigReadFaild = -1, // ��ȡ����ʧ��
	FileConfigNoRead = 0,     // ��û�ж�ȡ��
	FileConfigReadOk = 1,     // ��ȡ���óɹ�
};

// �ѽ����õ���������
typedef struct file_config_value
{
	char  Key[MAX_FILE_CONF_KL];   // ��ֵ
	char* Value[MAX_FILE_CONF_VC]; // ֵ�б�
	int   ValueCount;              // ֵ����
	file_config_value* Next;
} FileConfigValue, *PFileConfigValue;

class FileConfig
{
private:
	// 0-��δ��ȡ�����ļ� -1=�����ļ������� 1=�Ѷ�ȡ�����ļ�
	FileConfigReadFlag m_nReadFlag = FileConfigReadFlag::FileConfigNoRead;
	// ֵ����ָ��
	PFileConfigValue m_pValueLink = nullptr;
	// ֵ����ָ��.��ǰ
	PFileConfigValue m_pValueCurrent = nullptr;
	// ֵ����ָ��.���.���ڽ������.��������
	PFileConfigValue m_pValueMul = nullptr;
public:
	// ...
	FileConfig();
	// file=�����ļ�
	FileConfig(const char* file);
	// ����
	~FileConfig();
	// ��ȡ���� file=���õ��ļ�
	bool ReadConfig(const char* file);
	// �Ƿ��ȡ���óɹ�
	bool IsReadOk();
	// ��ȡֵ key=��ֵ
	const char* GetValue(const char* key);
	// ��ȡֵ key=��ֵ index=�ڼ���
	const char* GetValue(const char* key, int index);
	// ��ȡֵ�б� key=��ֵ ����length=�б�����
	char** GetValueMul(const char* key, int& count);
	// ��ȡֵ key=��ֵ
	int GetValue2Int(const char* key);
	// ��ȡֵ key=��ֵ index=�ڼ��� dv=Ĭ��ֵ
	int GetValue2Int(const char* key, int index, int dv=0);
	// ��ӡ������ֵ
	void PrintAll();
private:
	// ����һ������
	void ParseString(char* data, int length);
	// ���ö�ȡ��־ 0-��δ��ȡ�����ļ� -1=�����ļ������� 1=�Ѷ�ȡ�����ļ�
	void SetReadFlag(FileConfigReadFlag flag);
};