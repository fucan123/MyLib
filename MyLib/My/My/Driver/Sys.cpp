#include "Sys.h"
#include <stdio.h>

bool Sys::Install(const wchar_t* sName, const wchar_t* dName, const wchar_t* path)
{
	m_hScmanager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	m_hService = ::OpenService((SC_HANDLE)m_hScmanager, sName, SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
	if (!m_hService) {
		//printf("CreateService\n");
		m_hService = CreateService(
			(SC_HANDLE)m_hScmanager, sName, dName,
			SERVICE_ALL_ACCESS,
			SERVICE_KERNEL_DRIVER,
			SERVICE_DEMAND_START,
			SERVICE_ERROR_IGNORE,
			path,
			NULL, NULL, NULL, NULL, NULL);
	}
	else {
		//printf("OpenService\n");
		SERVICE_STATUS status;
		if (::QueryServiceStatus((SC_HANDLE)m_hService, &status) == FALSE) {
			//printf("无法获取服务状态 %d\n", GetLastError());

			::CloseServiceHandle((SC_HANDLE)m_hService);
			::CloseServiceHandle((SC_HANDLE)m_hScmanager);
			return false;
		}

		if (status.dwCurrentState == SERVICE_RUNNING) {
			//printf("服务是启动状态\n");
			return true;
		}
		//printf("status:%08X\n", status.dwCurrentState);
	}
	

	if (!m_hService) {
		//printf("创建驱动文件失败:%d\n", GetLastError());
	}
	//printf("Last Error:%d %ws\n", GetLastError(), path);

	bool start = StartService((SC_HANDLE)m_hService, NULL, NULL);
	if (!start) {
		//printf("no start:%d\n", GetLastError());
		DeleteService((SC_HANDLE)m_hService);
		CloseServiceHandle((SC_HANDLE)m_hService);
		CloseServiceHandle((SC_HANDLE)m_hScmanager);
		printf("Last Error:%d\n", GetLastError());
	}

	//printf("Last Error:%d\n", GetLastError());

	return start;
}

bool Sys::UnInstall()
{
	SERVICE_STATUS status;
	ControlService((SC_HANDLE)m_hService, SERVICE_CONTROL_STOP, &status);
	bool del = DeleteService((SC_HANDLE)m_hService);

	del = CloseServiceHandle((SC_HANDLE)m_hService);
	del = CloseServiceHandle((SC_HANDLE)m_hScmanager) && del;

	return del;
}
