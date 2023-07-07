#include "kingsguard.h"

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