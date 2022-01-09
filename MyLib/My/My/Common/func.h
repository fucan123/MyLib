#pragma once
#include <Windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <winuser.h>

#define TEST_XXX 1
#if TEST_XXX == 1
#define DPrint(_X_) printf _X_
#else
#define DPrint(_X_)
#endif

// Բ����
#define PI 3.14159265358979323846f
// �Ƿ�������֮��
#define IS_BETEEN_AS(v,v2,c) ( (((v)-(c)<=(v2)) && (((v)+(c)>=(v2)))) )
// ����ֵ
#define MYABS(v) ((v)>=0?(v):-(v))


#pragma warning(disable: 4996)

// ��ȡSystem Dll���溯��
FARPROC GetSystemdllProcAddress(const char*  dll, const char* name);

// ��ȡntdll���溯��
FARPROC GetNtdllProcAddress(const char* name);

// �ж��ļ��Ƿ����
BOOL IsFileExist(const LPCTSTR csFile);

// �ж��ļ��Ƿ����
BOOL IsFileExistA(const char* csFile);

// �ж��ļ����Ƿ����
BOOL IsDirExist(const LPCTSTR csFile);

// �ж��ļ����Ƿ����
BOOL IsDirExistA(const char* csFile);

/*
  �ж��Ƿ�ʼǱ�����
*/
bool IsNoteBook();

// ��Ȩ
bool AdjustPrivileges();

BOOL WINAPI EnumWindowsProc(HWND hWnd, LPARAM lParam);
/*
  ͨ������ID��ȡ���ھ��
*/
HWND GetWindowByPid(DWORD dwProcessID);

/*
  ��ȡ��ǰ���̴��ڻ���ݽ���id
*/
HWND GetProcessWindow(DWORD dwProcessID=0);

/*
  ��ȡ������ID
*/
DWORD GetParentProcessID(DWORD pid=0);

/*
  ���ݽ������ƻ�ý���ID
  @pName ��������
  @return ����ID
*/
DWORD SGetProcessId(LPCTSTR pName);
/*
  ���ݽ������ƻ�ý���ID
  @pName ��������
  @Pids  �洢�������ַ
  @return ��������
*/
DWORD SGetProcessIds(LPCTSTR pName, DWORD Pids[], DWORD maxlen);

/*
��ȡ��������
*/
BOOL GetProcessName(OUT LPTSTR pName, DWORD pid=0);

/*
  ��ȡĳģ�����ַ
  @dwPID ����ID
  @szModuleName ģ������
  @szModuleName ����ģ���С�ĵ�ַ
  @return ģ���ַ
*/
BYTE* GetModuleBaseAddr(DWORD dwPID, LPCTSTR szModuleName, DWORD* dwModuleSize=NULL);

// ��ȡģ���ַ
HMODULE EnumModuleBaseAddr(DWORD dwPID, LPCTSTR szModuleName, DWORD* dwModuleSize = NULL);

void PrintModules(DWORD dwPID);

// ��ȡĳ����ģ��
HMODULE GethModule(DWORD dwPID, LPCTSTR szModuleName);

/*
  �ж�DLL�Ƿ����ĳһ����
  @dwPID ����ID
  @szDllPath DLL����
  @return bool
*/
BOOL CheckDllInProcess(DWORD dwPID, LPCTSTR szDllPath);
BOOL CheckDllInProcess2(DWORD dwPID, LPCTSTR szShortName);
/*
  ��ָ���Ľ���ע����Ӧ��ģ��
  @dwPID Ŀ����̵�PID
  @szDllPath ��ע���dll������·��
*/
BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath, LPCTSTR szShorName, BOOL b64In32=FALSE);
BOOL InjectDllA(DWORD dwPID, const char* szDllPath, const char* szShorName, BOOL b64In32=FALSE);
/*
  ��ָ���Ľ���ж����Ӧ��ģ��
  @dwPID Ŀ����̵�PID
  @szDllPath ��ע���dll������·��,ע�⣺·����Ҫ�á�/�������桰\\��
  */
BOOL EjectDll(DWORD dwPID, LPCTSTR szDllPath);
/*
  ��ý���ռ���ڴ��С
  @ProcessId ����ID
*/
SIZE_T GetWorkingSetSize(DWORD ProcessId);
/* ������ű� */
FLOAT GetScale();
/*
  ��������ת��Ļ����
  @martix ����
  @wPos ��������
  @sPos ���浽����Ļ�����ַ
  @wh   ��Ļ���
*/
void WorldToScreen_OpenGL(FLOAT martix[4][4], FLOAT wPos[3], FLOAT sPos[3], FLOAT wh[2]);
void Tmp_OpenGL(FLOAT martix[4][4], FLOAT wPos[3], FLOAT nPos[3]);
/*
  ���3D�ռ�����֮��ľ���
*/
DOUBLE GetDistBy3D(FLOAT from_x, FLOAT from_y, FLOAT from_z, FLOAT to_x, FLOAT to_y, FLOAT to_z);
/*
  �������֮��Ķ�ά�Ƕ�
*/
DOUBLE GetAngle_XY(FLOAT from_x, FLOAT from_y, FLOAT to_x, FLOAT to_y);
/*
  ������ӽǶ�
*/
DOUBLE GetAngle_Z(FLOAT from_x, FLOAT from_y, FLOAT from_z, FLOAT to_x, FLOAT to_y, FLOAT to_z);