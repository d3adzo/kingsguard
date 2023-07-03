$arr =  "HKLM:\Software\Microsoft\Windows NT\CurrentVersion\Windows", "HKLM:\Software\Wow6432Node\Microsoft\Windows NT\CurrentVersion\Windows"

Foreach ($i in $arr)
{
    $acl = Get-Acl $i
    $rule = New-Object System.Security.AccessControl.RegistryAccessRule ("NT AUTHORITY\INTERACTIVE","SetValue","Deny")
    $acl.SetAccessRule($rule)
    echo $acl | format-list
    $acl | Set-Acl -Path $i

}