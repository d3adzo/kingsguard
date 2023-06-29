#include "kingsguard.h"

NTSTATUS WINAPI HookedNtOpenFile(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, ULONG ShareAccess, ULONG OpenOptions)
{
    if (DesiredAccess & DELETE && ObjectAttributes->ObjectName->Length > 3)
    {
        if (CheckExistsW(std::wstring(ObjectAttributes->ObjectName->Buffer), PATH, false))
            return STATUS_ACCESS_DENIED;
    }

    return OriginalNtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
}

