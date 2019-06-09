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

// 圆周率
#define PI 3.14159265358979323846f
// 是否在两者之间
#define IS_BETEEN_AS(v,v2,c) ( (((v)-(c)<=(v2)) && (((v)+(c)>=(v2)))) )
// 绝对值
#define MYABS(v) ((v)>=0?(v):-(v))


#pragma warning(disable: 4996)

// 判断文件是否存在
BOOL IsFileExist(const LPCTSTR csFile);
/*
  判断是否笔记本电脑
*/
bool IsNoteBook();

BOOL EnumWindowsProc(HWND hWnd, LPARAM lParam);
/*
  通过进程ID获取窗口句柄
*/
HWND GetHwndByPID(DWORD dwProcessID);
/*
  根据进程名称获得进程ID
  @pName 进程名称
  @return 进程ID
*/
DWORD SGetProcessId(LPCTSTR pName);
/*
  根据进程名称获得进程ID
  @pName 进程名称
  @Pids  存储的数组地址
  @return 进程数量
*/
DWORD SGetProcessIds(LPCTSTR pName, DWORD Pids[], DWORD maxlen);

/*
  获取某模块基地址
  @dwPID 进程ID
  @szModuleName 模块名称
  @szModuleName 保存模块大小的地址
  @return 模块地址
*/
BYTE* GetModuleBaseAddr(DWORD dwPID, LPCTSTR szModuleName, DWORD* dwModuleSize=NULL);

/*
  判断DLL是否存在某一进程
  @dwPID 进程ID
  @szDllPath DLL名称
  @return bool
*/
BOOL CheckDllInProcess(DWORD dwPID, LPCTSTR szDllPath);
/*
  向指定的进程注入相应的模块
  @dwPID 目标进程的PID
  @szDllPath 被注入的dll的完整路径
*/
BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath);
/*
  让指定的进程卸载相应的模块
  @dwPID 目标进程的PID
  @szDllPath 被注入的dll的完整路径,注意：路径不要用“/”来代替“\\”
  */
BOOL EjectDll(DWORD dwPID, LPCTSTR szDllPath);
/*
  获得进程占用内存大小
  @ProcessId 进程ID
*/
SIZE_T GetWorkingSetSize(DWORD ProcessId);
/* 获得缩放比 */
FLOAT GetScale();
/*
  世界坐标转屏幕坐标
  @martix 矩阵
  @wPos 世界坐标
  @sPos 保存到的屏幕坐标地址
  @wh   屏幕宽高
*/
void WorldToScreen_OpenGL(FLOAT martix[4][4], FLOAT wPos[3], FLOAT sPos[3], FLOAT wh[2]);
void Tmp_OpenGL(FLOAT martix[4][4], FLOAT wPos[3], FLOAT nPos[3]);
/*
  获得3D空间两点之间的距离
*/
DOUBLE GetDistBy3D(FLOAT from_x, FLOAT from_y, FLOAT from_z, FLOAT to_x, FLOAT to_y, FLOAT to_z);
/*
  获得两点之间的二维角度
*/
DOUBLE GetAngle_XY(FLOAT from_x, FLOAT from_y, FLOAT to_x, FLOAT to_y);
/*
  获得仰视角度
*/
DOUBLE GetAngle_Z(FLOAT from_x, FLOAT from_y, FLOAT from_z, FLOAT to_x, FLOAT to_y, FLOAT to_z);