#include "kingsguard.h"

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
    DWORD pid, ppid;
    int errorCode;
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
            pid = GetPPID(pid);
        }
    } while (processName.find("explorer") == std::string::npos);

    return true;
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