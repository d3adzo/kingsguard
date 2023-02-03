#include <windows.h>

const char* backupArray[] = {
    "backuppath",
    "newpath",
    "backuppath",
    "newpath",
    "backuppath",
    "newpath",
    "backuppath",
    "newpath",
};

void exec(const char* command)
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	
	CreateProcessA(NULL, (char*)command, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

    // test both TODO
    WaitForInputIdle(pi.hProcess, INFINITE);
    WaitForSingleObject(pi.hProcess, INFINITE);

    // clean up and exit
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

int iterateBackups()
{
    for (int i = 0; i < sizeof(backupArray) / sizeof(backupArray[0]); i+=2)
    if (!CopyFileA(backupArray[i], backupArray[i+1], TRUE)) // return 0 means it failed
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            return 1;
        }
    }
    return 0;
}

int main()
{
    for(;;)
    {
        // files watchdog
        if (iterateBackups) { return 1; }

        // install WMI Filter in thread
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&exec, "mofcomp.exe path_to_wizard.mof", 0, NULL); 

        // sleep 10 seconds
        Sleep(10000);
    }
}