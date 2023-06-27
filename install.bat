reg delete "hklm\software\microsoft\windows nt\currentversion\windows" /v LoadAppInit_DLLs /f
reg add "hklm\software\microsoft\windows nt\currentversion\windows" /v LoadAppInit_DLLs /t reg_dword /d 1 /f
reg add "hklm\software\microsoft\windows nt\currentversion\windows" /v RequireSignedAppInit_DLLs /t reg_dword /d 0 /f
reg add "hklm\software\wow6432node\microsoft\windows nt\currentversion\windows" /v RequireSignedAppInit_DLLs /t reg_dword /d 0 /f
reg add "hklm\software\wow6432node\microsoft\windows nt\currentversion\windows" /v AppInit_DLLs /t reg_sz /d "C:\users\administrator\desktop\kingsguard.dll" /f
reg add "hklm\software\microsoft\windows nt\currentversion\windows" /v AppInit_DLLs /t reg_sz /d "C:\users\administrator\desktop\kingsguard.dll" /f
