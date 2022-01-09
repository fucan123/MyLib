#include "ShowHook.h"

// 读取原始环境
void ShowHook::ReadOrigFunc(const char* name, void* address, bool print)
{
	if (address == nullptr) {
		if (print) {
			::printf("%s:address==nullptr.\n", name);
		}
		return;
	}

	OriginalFunc* pFunc = new OriginalFunc;
	memset(pFunc, 0, sizeof(OriginalFunc));
	strcpy(pFunc->Name, name);
	pFunc->Address = (PBYTE)address;

	PBYTE p = (PBYTE)address;
	for (int i = 0; i < MAX_FUNC_BYTE_SIZE; i++,p++) {
		pFunc->Codes[i] = *p;
		pFunc->Len++;

		if (*p == 0xC3) { // ret
			break;
		}
	}

	if (print) {
		::printf("%s read original func:%p-%p %d\n", name, address, p, pFunc->Len);
	}
	m_Funcs.push_back(pFunc);
}

// 显示被HOOK的函数
int ShowHook::Show(bool print)
{
	int count = 0;
	for (int i = 0; i < m_Funcs.size(); i++) {
		int hook_index = -1;
		for (int j = 0; j < m_Funcs[i]->Len; j++) {
			if (m_Funcs[i]->Address[j] != m_Funcs[i]->Codes[j]) {
				hook_index = j;
				break;
			}
		}

		if (hook_index > -1) {
			if (print) {
				::printf("%s is hoo (original|modrify)\n", m_Funcs[i]->Name);
				int x = 0;
				int y = hook_index + 10 <= m_Funcs[i]->Len ? hook_index + 10 : m_Funcs[i]->Len;
				for (x = 0; x < y; x++) {
					printf("%02X ", m_Funcs[i]->Codes[x]&0xff);
				}
				printf("\n");
				for (x = 0; x < y; x++) {
					printf("%02X ", m_Funcs[i]->Address[x] & 0xff);
				}
				printf("\n");
			}
			count++;
		}
	}
	return count;
}
