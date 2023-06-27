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

bool ExplorerChild(void)
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
        e = getProcessName(pid, fname, MAX_PATH);
        name = std::string(fname);
        pid = getParentPID(pid);
    } while (name.find("explorer") == std::string::npos); // TODO make it so it loads into explorer as well. good for testing now without.

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