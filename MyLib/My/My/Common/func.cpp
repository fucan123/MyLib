#include "func.h"
#include <stdio.h>
#include <math.h>
#include <tchar.h>

// 判断文件是否存在
BOOL IsFileExist(const LPCTSTR csFile)
{
	DWORD dwAttrib = GetFileAttributes(csFile);
	return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

bool IsNoteBook()
{
	SYSTEM_POWER_STATUS status;
	GetSystemPowerStatus(&status);
	return status.BatteryFlag != 128 && status.BatteryFlag != 255;
}

BOOL EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	DWORD param = *((DWORD*)lParam);
	DWORD pid;
	GetWindowThreadProcessId(hWnd, &pid);
	if (param == pid && GetParent(hWnd) == NULL) {
		LONG style = GetWindowLong(hWnd, GWL_STYLE);
		if ((style&WS_VISIBLE) && (style&WS_SYSMENU)) {
			*(DWORD*)lParam = (DWORD)hWnd;
			return FALSE;
		}
	}
	return TRUE;
}

//通过进程ID获取窗口句柄
HWND GetHwndByPID(DWORD dwProcessID)
{
	/*DWORD param = dwProcessID;
	if (!::EnumWindows(EnumWindowsProc, (LPARAM)&param)) {
		return (HWND)param;
	}*/
	return nullptr;
}

DWORD SGetProcessId(LPCTSTR pName)
{
	// 定义进程信息结构
	PROCESSENTRY32 pe32 = { sizeof(pe32) };

	// 创建系统当前进程快照
	HANDLE hProcessShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessShot == INVALID_HANDLE_VALUE)
		return 0;

	if (Process32First(hProcessShot, &pe32)) {
		do {
			//printf("%s(%d)\n", pe32.szExeFile, pe32.th32ProcessID);
			if (_tcsicmp(pName, pe32.szExeFile) == 0) {
				return pe32.th32ProcessID;
			}

		} while (Process32Next(hProcessShot, &pe32));
	}

	CloseHandle(hProcessShot);
	return 0;
}

DWORD SGetProcessIds(LPCTSTR pName, DWORD Pids[], DWORD maxlen)
{

	// 进程数量
	DWORD count = 0;
	// 定义进程信息结构
	PROCESSENTRY32 pe32 = { sizeof(pe32) };
	// 创建系统当前进程快照
	HANDLE hProcessShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessShot == INVALID_HANDLE_VALUE)
		return 0;

	if (Process32First(hProcessShot, &pe32)) {
		do {
			if (count >= maxlen)
				break;

			if (_tcsicmp(pName, pe32.szExeFile) == 0) {
				Pids[count] = pe32.th32ProcessID;
				count++;
			}

		} while (Process32Next(hProcessShot, &pe32));
	}

	CloseHandle(hProcessShot);
	return count;
}

/*
  获取某模块基地址
  @dwPID 进程ID
  @szModuleName 模块名称
  @dwModuleSize 保存模块大小的地址
  @return 模块地址
*/
BYTE* GetModuleBaseAddr(DWORD dwPID, LPCTSTR szModuleName, DWORD* dwModuleSize)
{
	BOOL                    bMore = FALSE;
	HANDLE                  hSnapshot = INVALID_HANDLE_VALUE;
	MODULEENTRY32           me = { sizeof(me), };

	printf("Last Error:%d\n", GetLastError());
	if (INVALID_HANDLE_VALUE ==
		(hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID))) { // 获得进程的快照
		printf("无法获得权限:%d\n", GetLastError());
		return NULL;
	}

	int n = 0;
	bMore = Module32FirstW(hSnapshot, &me); // 遍历进程内得的所有模块
	for (; bMore; bMore = Module32Next(hSnapshot, &me), n++) {
		//_tprintf(L"%ws %ws\n", me.szModule, me.szExePath);
		if (_tcsstr(me.szModule, szModuleName) || _tcsstr(me.szExePath, szModuleName)) {
			//printf("获得\n");
			if (dwModuleSize) {
				*dwModuleSize = me.modBaseSize;
			}
			return me.modBaseAddr;
		}
	}
	//printf("Last Error:%d 数量:%d\n", GetLastError(), n);
	CloseHandle(hSnapshot);
	return NULL;
}


/*
  判断DLL是否存在某一进程
  @dwPID 进程ID
  @szDllPath DLL名称
  @return bool
*/
BOOL CheckDllInProcess(DWORD dwPID, LPCTSTR szDllPath)
{
	BOOL                    bMore = FALSE;
	HANDLE                  hSnapshot = INVALID_HANDLE_VALUE;
	MODULEENTRY32           me = { sizeof(me), };

	if (INVALID_HANDLE_VALUE ==
		(hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID))) { // 获得进程的快照
		return FALSE;
	}
	bMore = Module32First(hSnapshot, &me); // 遍历进程内得的所有模块
	for (; bMore; bMore = Module32Next(hSnapshot, &me)) {
		//_tprintf(L"%ws %ws\n", me.szModule, me.szExePath);
		if (!_tcsicmp(me.szModule, szDllPath) || !_tcsicmp(me.szExePath, szDllPath)) { //模块名或含路径的名相符
			CloseHandle(hSnapshot);
			return TRUE;
		}
		me.modBaseAddr;
	}
	CloseHandle(hSnapshot);
	return FALSE;
}

/*
  向指定的进程注入相应的模块
  @dwPID 目标进程的PID
  @szDllPath 被注入的dll的完整路径
*/
BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath)
{
	HANDLE                  h_token;
	HANDLE                  hProcess = NULL; // 保存目标进程的句柄
	LPVOID                  pRemoteBuf = NULL; // 目标进程开辟的内存的起始地址
	DWORD                   dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR); // 开辟的内存的大小
	LPTHREAD_START_ROUTINE  pThreadProc = NULL; // loadLibreayW函数的起始地址
	HMODULE                 hMod = NULL; // kernel32.dll模块的句柄
	BOOL                    bRet = FALSE;
	HANDLE                  rt = NULL;

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID))) { // 打开目标进程，获得句柄
		printf("InjectDll() : OpenProcess(%d) failed!!! [%d]\n",
			dwPID, GetLastError());
		goto INJECTDLL_EXIT;
	}
	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize,
		MEM_COMMIT, PAGE_READWRITE);
	if (pRemoteBuf == NULL) { //在目标进程空间开辟一块内存
		printf("InjectDll() : VirtualAllocEx() failed!!! [%d]\n",
			GetLastError());
		goto INJECTDLL_EXIT;
	}
	if (!WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL)) { // 向开辟的内存复制dll的路径
		printf("InjectDll() : WriteProcessMemory() failed!!! [%d]\n",
			GetLastError());
		goto INJECTDLL_EXIT;
	}
	hMod = GetModuleHandle(L"kernel32.dll"); // 获得本进程kernel32.dll的模块句柄
	if (hMod == NULL) {
		printf("InjectDll() : GetModuleHandle(\"kernel32.dll\") failed!!! [%d]\n",
			GetLastError());
		goto INJECTDLL_EXIT;
	}
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW"); // 获得LoadLibraryW函数的起始地址
	if (pThreadProc == NULL) {
		printf("InjectDll() : GetProcAddress(\"LoadLibraryW\") failed!!! [%d]\n",
			GetLastError());
		goto INJECTDLL_EXIT;
	}
	rt = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
	printf("进程->ID: %d, %ws(%d) LoadLibraryW: %08x --- rt: %08X\n", dwPID, szDllPath, dwBufSize, pThreadProc, rt);
	if (!rt) { // 执行远程线程
		printf("InjectDll() : MyCreateRemoteThread() failed!!!\n");
		goto INJECTDLL_EXIT;
	}
	printf("Last Error:%d\n", GetLastError());
	Sleep(100);
INJECTDLL_EXIT:
	bRet = CheckDllInProcess(dwPID, szDllPath); // 确认结果
	if (pRemoteBuf)
		VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
	if (hProcess)
		CloseHandle(hProcess);
	return bRet;
}

/*
  让指定的进程卸载相应的模块
  @dwPID 目标进程的PID
  @szDllPath 被注入的dll的完整路径,注意：路径不要用“/”来代替“\\”
  */
BOOL EjectDll(DWORD dwPID, LPCTSTR szDllPath)
{
	BOOL                    bMore = FALSE, bFound = FALSE, bRet = FALSE;
	HANDLE                  hSnapshot = INVALID_HANDLE_VALUE;
	HANDLE                  hProcess = NULL;
	MODULEENTRY32           me = { sizeof(me), };
	LPTHREAD_START_ROUTINE  pThreadProc = NULL;
	HMODULE                 hMod = NULL;
	TCHAR                   szProcName[MAX_PATH] = { 0, };
	if (INVALID_HANDLE_VALUE == (hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID))) {
		_tprintf(L"EjectDll() : CreateToolhelp32Snapshot(%d) failed!!! [%d]\n",
			dwPID, GetLastError());
		goto EJECTDLL_EXIT;
	}
	bMore = Module32First(hSnapshot, &me);
	for (; bMore; bMore = Module32Next(hSnapshot, &me)) { // 查找模块句柄
		if (!_tcsicmp(me.szModule, szDllPath) ||
			!_tcsicmp(me.szExePath, szDllPath)) {
			bFound = TRUE;
			break;
		}
	}
	if (!bFound) {
		_tprintf(L"EjectDll() : There is not %s module in process(%d) memory!!!\n",
			szDllPath, dwPID);
		goto EJECTDLL_EXIT;
	}
	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID))) {
		_tprintf(L"EjectDll() : OpenProcess(%d) failed!!! [%d]\n",
			dwPID, GetLastError());
		goto EJECTDLL_EXIT;
	}
	hMod = GetModuleHandle(L"kernel32.dll");
	if (hMod == NULL) {
		_tprintf(L"EjectDll() : GetModuleHandle(\"kernel32.dll\") failed!!! [%d]\n",
			GetLastError());
		goto EJECTDLL_EXIT;
	}
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "FreeLibrary");
	if (pThreadProc == NULL) {
		_tprintf(L"EjectDll() : GetProcAddress(\"FreeLibrary\") failed!!! [%d]\n",
			GetLastError());
		goto EJECTDLL_EXIT;
	}
	if (!CreateRemoteThread(hProcess, NULL, 0, pThreadProc, me.modBaseAddr, 0, NULL)) {
		_tprintf(L"EjectDll() : MyCreateRemoteThread() failed!!!\n");
		goto EJECTDLL_EXIT;
	}
	printf("FreeLibrary\n");
	bRet = CheckDllInProcess(dwPID, szDllPath); // 确认结果
EJECTDLL_EXIT:
	if (hProcess)
		CloseHandle(hProcess);
	if (hSnapshot != INVALID_HANDLE_VALUE)
		CloseHandle(hSnapshot);
	return bRet == FALSE;
}

SIZE_T GetWorkingSetSize(DWORD ProcessId)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, ProcessId);
	if (!hProcess)
		return 0;

	PROCESS_MEMORY_COUNTERS process_mem_counters = { 0 };
	process_mem_counters.cb = sizeof(PROCESS_MEMORY_COUNTERS);
	DWORD bRet = GetProcessMemoryInfo(hProcess, &process_mem_counters, sizeof(PROCESS_MEMORY_COUNTERS));

	if (hProcess)
		CloseHandle(hProcess);
	return process_mem_counters.WorkingSetSize / 1024;
}

FLOAT GetScale()
{
	SetProcessDPIAware();
	return (GetDeviceCaps(::GetDC(NULL), LOGPIXELSX)) / 100.0f / 0.96f;
}

void WorldToScreen_OpenGL(FLOAT martix[4][4], FLOAT wPos[3], FLOAT sPos[3], FLOAT wh[2])
{
	FLOAT x, y, z, w;
	x = martix[0][0] * wPos[0] + martix[1][0] * wPos[1] + martix[2][0] * wPos[2] + martix[3][0];
	y = martix[0][1] * wPos[0] + martix[1][1] * wPos[1] + martix[2][1] * wPos[2] + martix[3][1];
	z = martix[0][2] * wPos[0] + martix[1][2] * wPos[1] + martix[2][2] * wPos[2] + martix[3][2];
	w = martix[0][3] * wPos[0] + martix[1][3] * wPos[1] + martix[2][3] * wPos[2] + martix[3][3];
	if (w > 0) {
		x = x / w;
		y = y / w;

		sPos[0] = (wh[0] / 2.0f * x) + (x + (wh[0] / 2.0f));
		sPos[1] = wh[1] - ((wh[1] / 2.0f * y) + (y + (wh[1] / 2.0f)));
		sPos[2] = z;
	}
	else {
		sPos[0] = -1000.0f;
		sPos[1] = -1000.0f;
		sPos[2] = 0.0f;
	}
}

void Tmp_OpenGL(FLOAT martix[4][4], FLOAT wPos[3], FLOAT nPos[3])
{
	FLOAT x, y, z, w;
	x = martix[0][0] * wPos[0] + martix[1][0] * wPos[1] + martix[2][0] * wPos[2] + martix[3][0];
	y = martix[0][1] * wPos[0] + martix[1][1] * wPos[1] + martix[2][1] * wPos[2] + martix[3][1];
	z = martix[0][2] * wPos[0] + martix[1][2] * wPos[1] + martix[2][2] * wPos[2] + martix[3][2];
	w = martix[0][3] * wPos[0] + martix[1][3] * wPos[1] + martix[2][3] * wPos[2] + martix[3][3];
	if (w > 0) {
		x /= w;
		y /= w;
		z /= w;
	}
	nPos[0] = x;
	nPos[1] = y;
	nPos[2] = z;
}

DOUBLE GetDistBy3D(FLOAT from_x, FLOAT from_y, FLOAT from_z, FLOAT to_x, FLOAT to_y, FLOAT to_z)
{
	DOUBLE dx = from_x - to_x, dy = from_y - to_y, dz = from_z - to_z;
	return sqrt((dx * dx) + (dy * dy) + (dz * dz));
}

DOUBLE GetAngle_XY(FLOAT from_x, FLOAT from_y, FLOAT to_x, FLOAT to_y)
{
	if (from_x == to_x || from_y == to_y)
		return 0;

	return atan((to_y - from_y) / (to_x - from_x)) * 180.0f / PI; //转换为角度值
}

DOUBLE GetAngle_Z(FLOAT from_x, FLOAT from_y, FLOAT from_z, FLOAT to_x, FLOAT to_y, FLOAT to_z)
{
	DOUBLE dx = to_x - from_x;
	DOUBLE dy = to_y - from_y;
	DOUBLE dz = to_z - from_z;
	DOUBLE sqrt_xy = sqrt((dx * dx) + (dy * dy));
	if (sqrt_xy == 0)
		return 0;

	DOUBLE angle = atan(dz / sqrt_xy) * 180.0f / PI; //转换为角度值
	return angle > 0 ? angle : -angle;
}
