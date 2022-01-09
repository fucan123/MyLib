#include "DownFile.h"

DownProgress DownloadProgress::dpFunc = nullptr;

bool DownFile(const char* remote_file, const char* local_file, DownProgress func)
{
	DownloadProgress progress;
	progress.dpFunc = func;
	HRESULT hr = URLDownloadToFileA(NULL, remote_file, local_file, NULL, static_cast<IBindStatusCallback*>(&progress));
	return SUCCEEDED(hr);
}
