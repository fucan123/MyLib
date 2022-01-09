#pragma once
#include <Windows.h>

#define NT_SUCCESS(x) ((x) >= 0)
#define ProcessBasicInformation 0

typedef enum _THREADINFOCLASS {
	ThreadBasicInformation,
	ThreadTimes,
	ThreadPriority,
	ThreadBasePriority,
	ThreadAffinityMask,
	ThreadImpersonationToken,
	ThreadDescriptorTableEntry,
	ThreadEnableAlignmentFaultFixup,
	ThreadEventPair_Reusable,
	ThreadQuerySetWin32StartAddress,
	ThreadZeroTlsCell,
	ThreadPerformanceCount,
	ThreadAmILastThread,
	ThreadIdealProcessor,
	ThreadPriorityBoost,
	ThreadSetTlsArrayAddress,
	ThreadIsIoPending,
	ThreadHideFromDebugger,
	ThreadBreakOnTermination,
	MaxThreadInfoClass
} THREADINFOCLASS;

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
#ifdef MIDL_PASS
	[size_is(MaximumLength / 2), length_is((Length) / 2)] USHORT* Buffer;
#else // MIDL_PASS
	_Field_size_bytes_part_opt_(MaximumLength, Length) PWCH   Buffer;
#endif // MIDL_PASS
} UNICODE_STRING;

typedef struct _LDR_DATA_TABLE_ENTRY                         // 24 elements, 0xE0 bytes (sizeof) 
{
	/*0x000*/     struct _LIST_ENTRY InLoadOrderLinks;                     // 2 elements, 0x10 bytes (sizeof)  
	/*0x010*/     struct _LIST_ENTRY InMemoryOrderLinks;                   // 2 elements, 0x10 bytes (sizeof)  
	/*0x020*/     struct _LIST_ENTRY InInitializationOrderLinks;           // 2 elements, 0x10 bytes (sizeof)  
	/*0x030*/     VOID* DllBase;
	/*0x038*/     VOID* EntryPoint;
	/*0x040*/     ULONG32      SizeOfImage;
	/*0x044*/     UINT8        _PADDING0_[0x4];
	/*0x048*/     struct _UNICODE_STRING FullDllName;                      // 3 elements, 0x10 bytes (sizeof)  
	/*0x058*/     struct _UNICODE_STRING BaseDllName;                      // 3 elements, 0x10 bytes (sizeof)  
}LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

typedef struct _CURDIR              // 2 elements, 0x18 bytes (sizeof) 
{
	/*0x000*/     struct _UNICODE_STRING DosPath; // 3 elements, 0x10 bytes (sizeof) 
	/*0x010*/     VOID* Handle;
}CURDIR, * PCURDIR;

typedef struct _RTL_USER_PROCESS_PARAMETERS                // 30 elements, 0x400 bytes (sizeof) 
{
	/*0x000*/     ULONG32      MaximumLength;
	/*0x004*/     ULONG32      Length;
	/*0x008*/     ULONG32      Flags;
	/*0x00C*/     ULONG32      DebugFlags;
	/*0x010*/     VOID* ConsoleHandle;
	/*0x018*/     ULONG32      ConsoleFlags;
	/*0x01C*/     UINT8        _PADDING0_[0x4];
	/*0x020*/     VOID* StandardInput;
	/*0x028*/     VOID* StandardOutput;
	/*0x030*/     VOID* StandardError;
	/*0x038*/     struct _CURDIR CurrentDirectory;                       // 2 elements, 0x18 bytes (sizeof)   
	/*0x050*/     struct _UNICODE_STRING DllPath;                        // 3 elements, 0x10 bytes (sizeof)   
	/*0x060*/     struct _UNICODE_STRING ImagePathName;                  // 3 elements, 0x10 bytes (sizeof)   
	/*0x070*/     struct _UNICODE_STRING CommandLine;                    // 3 elements, 0x10 bytes (sizeof)   
	/*0x080*/     VOID* Environment;
	/*0x088*/     ULONG32      StartingX;
	/*0x08C*/     ULONG32      StartingY;
	/*0x090*/     ULONG32      CountX;
	/*0x094*/     ULONG32      CountY;
	/*0x098*/     ULONG32      CountCharsX;
	/*0x09C*/     ULONG32      CountCharsY;
	/*0x0A0*/     ULONG32      FillAttribute;
	/*0x0A4*/     ULONG32      WindowFlags;
	/*0x0A8*/     ULONG32      ShowWindowFlags;
	/*0x0AC*/     UINT8        _PADDING1_[0x4];
	/*0x0B0*/     struct _UNICODE_STRING WindowTitle;                    // 3 elements, 0x10 bytes (sizeof)   
	/*0x0C0*/     struct _UNICODE_STRING DesktopInfo;                    // 3 elements, 0x10 bytes (sizeof)   
	/*0x0D0*/     struct _UNICODE_STRING ShellInfo;                      // 3 elements, 0x10 bytes (sizeof)   
	/*0x0E0*/     struct _UNICODE_STRING RuntimeData;                    // 3 elements, 0x10 bytes (sizeof)   
}RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;


typedef struct _PEB_LDR_DATA                            // 9 elements, 0x58 bytes (sizeof) 
{
	/*0x000*/     ULONG32      Length;
	/*0x004*/     UINT8        Initialized;
	/*0x005*/     UINT8        _PADDING0_[0x3];
	/*0x008*/     VOID* SsHandle;
	/*0x010*/     struct _LIST_ENTRY InLoadOrderModuleList;           // 2 elements, 0x10 bytes (sizeof) 
	/*0x020*/     struct _LIST_ENTRY InMemoryOrderModuleList;         // 2 elements, 0x10 bytes (sizeof) 
	/*0x030*/     struct _LIST_ENTRY InInitializationOrderModuleList; // 2 elements, 0x10 bytes (sizeof) 
	/*0x040*/     VOID* EntryInProgress;
	/*0x048*/     UINT8        ShutdownInProgress;
	/*0x049*/     UINT8        _PADDING1_[0x7];
	/*0x050*/     VOID* ShutdownThreadId;
}PEB_LDR_DATA, * PPEB_LDR_DATA;


typedef struct _PEB                                                                               // 91 elements, 0x380 bytes (sizeof) 
{
	/*0x000*/     UINT8        InheritedAddressSpace;
	/*0x001*/     UINT8        ReadImageFileExecOptions;
	/*0x002*/     UINT8        BeingDebugged;
	union                                                                                         // 2 elements, 0x1 bytes (sizeof)    
	{
		/*0x003*/         UINT8        BitField;
		struct                                                                                    // 6 elements, 0x1 bytes (sizeof)    
		{
			/*0x003*/             UINT8        ImageUsesLargePages : 1;                                                 // 0 BitPosition                     
			/*0x003*/             UINT8        IsProtectedProcess : 1;                                                  // 1 BitPosition                     
			/*0x003*/             UINT8        IsLegacyProcess : 1;                                                     // 2 BitPosition                     
			/*0x003*/             UINT8        IsImageDynamicallyRelocated : 1;                                         // 3 BitPosition                     
			/*0x003*/             UINT8        SkipPatchingUser32Forwarders : 1;                                        // 4 BitPosition                     
			/*0x003*/             UINT8        SpareBits : 3;                                                           // 5 BitPosition                     
		};
	};
	/*0x008*/     VOID* Mutant;
	/*0x010*/     VOID* ImageBaseAddress;
	/*0x018*/     struct _PEB_LDR_DATA* Ldr;
	/*0x020*/     struct _RTL_USER_PROCESS_PARAMETERS* ProcessParameters;
}PEB, * PPEB;

typedef LONG NTSTATUS;

typedef NTSTATUS(NTAPI *pfnNtWow64QueryInformationProcess64)(
	IN HANDLE ProcessHandle,
	IN ULONG ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL
);

typedef NTSTATUS(NTAPI *pfnNtWow64ReadVirtualMemory64)(
	IN HANDLE ProcessHandle,
	IN PVOID64 BaseAddress,
	OUT PVOID Buffer,
	IN ULONG64 Size,
	OUT PULONG64 NumberOfBytesRead
);

typedef
NTSTATUS(WINAPI *pfnNtQueryInformationProcess)
(HANDLE ProcessHandle, ULONG ProcessInformationClass,
	PVOID ProcessInformation, UINT32 ProcessInformationLength,
	UINT32* ReturnLength);

typedef 
NTSTATUS(WINAPI *pfnNtQuerySetInformationThread)(
	IN HANDLE          ThreadHandle,
	IN THREADINFOCLASS ThreadInformationClass,
	OUT PVOID          ThreadInformation,
	IN ULONG           ThreadInformationLength
	);

typedef 
NTSTATUS(WINAPI *pfnZwSetInformationThread)(
	IN HANDLE          ThreadHandle,
	IN THREADINFOCLASS ThreadInformationClass,
	OUT PVOID          ThreadInformation,
	IN ULONG           ThreadInformationLength,
	OUT PULONG         ReturnLength
	);

typedef struct _PROCESS_BASIC_INFORMATION32 {
	NTSTATUS ExitStatus;
	UINT32 PebBaseAddress;
	UINT32 AffinityMask;
	UINT32 BasePriority;
	UINT32 UniqueProcessId;
	UINT32 InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION32;

typedef struct _UNICODE_STRING32
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;
} UNICODE_STRING32, *PUNICODE_STRING32;

typedef struct _PEB32
{
	UCHAR InheritedAddressSpace;
	UCHAR ReadImageFileExecOptions;
	UCHAR BeingDebugged;
	UCHAR BitField;
	ULONG Mutant;
	ULONG ImageBaseAddress;
	ULONG Ldr;
	ULONG ProcessParameters;
	ULONG SubSystemData;
	ULONG ProcessHeap;
	ULONG FastPebLock;
	ULONG AtlThunkSListPtr;
	ULONG IFEOKey;
	ULONG CrossProcessFlags;
	ULONG UserSharedInfoPtr;
	ULONG SystemReserved;
	ULONG AtlThunkSListPtr32;
	ULONG ApiSetMap;
} PEB32, *PPEB32;

typedef struct _PEB_LDR_DATA32
{
	ULONG Length;
	BOOLEAN Initialized;
	ULONG SsHandle;
	LIST_ENTRY32 InLoadOrderModuleList;
	LIST_ENTRY32 InMemoryOrderModuleList;
	LIST_ENTRY32 InInitializationOrderModuleList;
	ULONG EntryInProgress;
} PEB_LDR_DATA32, *PPEB_LDR_DATA32;

typedef struct _LDR_DATA_TABLE_ENTRY32
{
	LIST_ENTRY32 InLoadOrderLinks;
	LIST_ENTRY32 InMemoryOrderModuleList;
	LIST_ENTRY32 InInitializationOrderModuleList;
	ULONG DllBase;
	ULONG EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING32 FullDllName;
	UNICODE_STRING32 BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union
	{
		LIST_ENTRY32 HashLinks;
		ULONG SectionPointer;
	};
	ULONG CheckSum;
	union
	{
		ULONG TimeDateStamp;
		ULONG LoadedImports;
	};
	ULONG EntryPointActivationContext;
	ULONG PatchInformation;
} LDR_DATA_TABLE_ENTRY32, *PLDR_DATA_TABLE_ENTRY32;

typedef struct _PROCESS_BASIC_INFORMATION64 {
	NTSTATUS ExitStatus;
	UINT32 Reserved0;
	UINT64 PebBaseAddress;
	UINT64 AffinityMask;
	UINT32 BasePriority;
	UINT32 Reserved1;
	UINT64 UniqueProcessId;
	UINT64 InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION64;
typedef struct _PEB64
{
	UCHAR InheritedAddressSpace;
	UCHAR ReadImageFileExecOptions;
	UCHAR BeingDebugged;
	UCHAR BitField;
	ULONG64 Mutant;
	ULONG64 ImageBaseAddress;
	ULONG64 Ldr;
	ULONG64 ProcessParameters;
	ULONG64 SubSystemData;
	ULONG64 ProcessHeap;
	ULONG64 FastPebLock;
	ULONG64 AtlThunkSListPtr;
	ULONG64 IFEOKey;
	ULONG64 CrossProcessFlags;
	ULONG64 UserSharedInfoPtr;
	ULONG SystemReserved;
	ULONG AtlThunkSListPtr32;
	ULONG64 ApiSetMap;
} PEB64, *PPEB64;

typedef struct _PEB_LDR_DATA64
{
	ULONG Length;
	BOOLEAN Initialized;
	ULONG64 SsHandle;
	LIST_ENTRY64 InLoadOrderModuleList;
	LIST_ENTRY64 InMemoryOrderModuleList;
	LIST_ENTRY64 InInitializationOrderModuleList;
	ULONG64 EntryInProgress;
} PEB_LDR_DATA64, *PPEB_LDR_DATA64;

typedef struct _UNICODE_STRING64
{
	USHORT Length;
	USHORT MaximumLength;
	ULONG64 Buffer;
} UNICODE_STRING64, *PUNICODE_STRING64;

typedef struct _LDR_DATA_TABLE_ENTRY64
{
	LIST_ENTRY64 InLoadOrderLinks;
	LIST_ENTRY64 InMemoryOrderModuleList;
	LIST_ENTRY64 InInitializationOrderModuleList;
	ULONG64 DllBase;
	ULONG64 EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING64 FullDllName;
	UNICODE_STRING64 BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union
	{
		LIST_ENTRY64 HashLinks;
		ULONG64 SectionPointer;
	};
	ULONG CheckSum;
	union
	{
		ULONG TimeDateStamp;
		ULONG64 LoadedImports;
	};
	ULONG64 EntryPointActivationContext;
	ULONG64 PatchInformation;
} LDR_DATA_TABLE_ENTRY64, *PLDR_DATA_TABLE_ENTRY64;

// 伪装
void FakeCsrss();
// 获得进程PEB
ULONG32 GetPebBaseAddress32(DWORD pid=0);
// 获得进程PEB
ULONG64 GetPebBaseAddress64(DWORD pid=0);
// 进程是否调试
BOOL IsDebugger32(ULONG32 peb=0);
// 进程是否调试
BOOL IsDebugger64(ULONG64 peb = 0);
// 清除调试标志
VOID ZeroDebugger32(ULONG32 peb);
// 清除调试标志
VOID ZeroDebugger64(ULONG64 peb);
// 获取NtGlobalFlag
ULONG GetNtGlobalFlag32(ULONG32 peb);
// 获取NyGlobalFlag
ULONG GetNtGlobalFlag64(ULONG64 peb);
// 获取ntdll里面函数
FARPROC GetNtdllProcAddress_Self(LPCSTR name);