#pragma once

class Sys
{
public:
	void* m_hScmanager;
	void* m_hService;
public:
	Sys(): m_hScmanager(nullptr), m_hService(nullptr) {}
	bool Install(const wchar_t* sName, const wchar_t* dName, const wchar_t* path);
	bool UnInstall();
};