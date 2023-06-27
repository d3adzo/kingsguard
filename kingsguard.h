#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winternl.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <string>
#include <cwctype>
#include "MinHook.h"

#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#define STATUS_OBJECT_NAME_NOT_FOUND ((NTSTATUS)0xC0000034)
#define STATUS_ACCESS_DENIED ((NTSTATUS)0xC0000022)

typedef struct _MY_SYSTEM_PROCESS_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER Reserved[3];
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
    ULONG BasePriority;
    HANDLE ProcessId;
    HANDLE InheritedFromProcessId;
} MY_SYSTEM_PROCESS_INFORMATION, *PMY_SYSTEM_PROCESS_INFORMATION;

typedef enum _NT_KEY_INFORMATION_CLASS
{
    KeyBasicInformation,
    KeyNodeInformation,
    KeyFullInformation,
    KeyNameInformation
} NT_KEY_INFORMATION_CLASS;

typedef struct _NT_KEY_VALUE_BASIC_INFORMATION
{
    ULONG TitleIndex;
    ULONG Type;
    ULONG NameLength;
    WCHAR Name[1];
} NT_KEY_VALUE_BASIC_INFORMATION, *PNT_KEY_VALUE_BASIC_INFORMATION;

typedef enum _NT_KEY_VALUE_INFORMATION_CLASS
{
    KeyValueBasicInformation,
    KeyValueFullInformation,
    KeyValuePartialInformation
} NT_KEY_VALUE_INFORMATION_CLASS;

typedef struct _NT_KEY_VALUE_FULL_INFORMATION
{
    ULONG TitleIndex;
    ULONG Type;
    ULONG DataOffset;
    ULONG DataLength;
    ULONG NameLength;
    WCHAR Name[1];
} NT_KEY_VALUE_FULL_INFORMATION, *PNT_KEY_VALUE_FULL_INFORMATION;

typedef NTSTATUS(WINAPI *PNT_QUERY_SYSTEM_INFORMATION)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength);

typedef NTSTATUS(WINAPI *pNtTerminateProcess)(
    HANDLE hProcess,
    UINT code);

typedef NTSTATUS(WINAPI *pNtEnumerateValueKey)(
    HANDLE key,
    ULONG index,
    NT_KEY_VALUE_INFORMATION_CLASS keyValueInformationClass,
    LPVOID keyValueInformation,
    ULONG keyValueInformationLength,
    PULONG resultLength);

typedef NTSTATUS(WINAPI *pNtQueryValueKey)(
    HANDLE KeyHandle,
    PUNICODE_STRING ValueName,
    NT_KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    PVOID KeyValueInformation,
    ULONG Length,
    PULONG ResultLength);

typedef NTSTATUS(WINAPI *pNtOpenFile)(
    PHANDLE FileHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK IoStatusBlock,
    ULONG ShareAccess,
    ULONG OpenOptions);

PNT_QUERY_SYSTEM_INFORMATION OriginalNtQuerySystemInformation = nullptr;
pNtTerminateProcess OriginalNtTerminateProcess = nullptr;
pNtEnumerateValueKey OriginalNtEnumerateValueKey = nullptr;
pNtQueryValueKey OriginalNtQueryValueKey = nullptr;
pNtOpenFile OriginalNtOpenFile = nullptr;