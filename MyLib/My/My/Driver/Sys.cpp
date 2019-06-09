#include "Sys.h"
#include <Windows.h>
#include <stdio.h>

bool Sys::Install(const wchar_t* sName, const wchar_t* dName, const wchar_t* path)
{
	m_hScmanager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	m_hService = CreateService(
		(SC_HANDLE)m_hScmanager, sName, dName,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_IGNORE,
		path,
		NULL,NULL,NULL,NULL,NULL);

	//printf("Last Error:%d\n", GetLastError());

	bool start = StartService((SC_HANDLE)m_hService, NULL, NULL);
	if (!start) {
		DeleteService((SC_HANDLE)m_hService);
		CloseServiceHandle((SC_HANDLE)m_hService);
		CloseServiceHandle((SC_HANDLE)m_hScmanager);
	}

	//printf("Last Error:%d\n", GetLastError());

	return start;
}

bool Sys::UnInstall()
{
	SERVICE_STATUS status;
	ControlService((SC_HANDLE)m_hService, SERVICE_CONTROL_STOP, &status);
	bool del = DeleteService((SC_HANDLE)m_hService);

	CloseServiceHandle((SC_HANDLE)m_hService);
	CloseServiceHandle((SC_HANDLE)m_hScmanager);

	return del;
}
