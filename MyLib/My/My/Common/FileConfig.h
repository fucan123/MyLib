#pragma once
#define MAX_FILE_CONF_KL 64    // 最大键值长度
#define MAX_FILE_CONF_VC 64    // 最大值数量
#define MAX_FILE_CONF_VL 256   // 最大每个值长度

// 读取配置文件标志
enum class FileConfigReadFlag
{
	FileConfigReadFaild = -1, // 读取配置失败
	FileConfigNoRead = 0,     // 还没有读取了
	FileConfigReadOk = 1,     // 读取配置成功
};

// 已解析好的配置数据
typedef struct file_config_value
{
	char  Key[MAX_FILE_CONF_KL];   // 键值
	char* Value[MAX_FILE_CONF_VC]; // 值列表
	int   ValueCount;              // 值数量
	file_config_value* Next;
} FileConfigValue, *PFileConfigValue;

class FileConfig
{
private:
	// 0-还未读取配置文件 -1=配置文件不存在 1=已读取配置文件
	FileConfigReadFlag m_nReadFlag = FileConfigReadFlag::FileConfigNoRead;
	// 值链表指针
	PFileConfigValue m_pValueLink = nullptr;
	// 值链表指针.当前
	PFileConfigValue m_pValueCurrent = nullptr;
	// 值链表指针.多个.用于解析标记.其他无用
	PFileConfigValue m_pValueMul = nullptr;
public:
	// ...
	FileConfig();
	// file=配置文件
	FileConfig(const char* file);
	// 析构
	~FileConfig();
	// 读取配置 file=配置的文件
	bool ReadConfig(const char* file);
	// 是否读取配置成功
	bool IsReadOk();
	// 获取值 key=键值
	const char* GetValue(const char* key);
	// 获取值 key=键值 index=第几个
	const char* GetValue(const char* key, int index);
	// 获取值列表 key=键值 返回length=列表数量
	char** GetValueMul(const char* key, int& count);
	// 获取值 key=键值
	int GetValue2Int(const char* key);
	// 获取值 key=键值 index=第几个 dv=默认值
	int GetValue2Int(const char* key, int index, int dv=0);
	// 打印所有数值
	void PrintAll();
private:
	// 解析一行配置
	void ParseString(char* data, int length);
	// 设置读取标志 0-还未读取配置文件 -1=配置文件不存在 1=已读取配置文件
	void SetReadFlag(FileConfigReadFlag flag);
};