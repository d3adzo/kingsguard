
```

             />
            /<
           /<
 |\_______{o}----------------------------------------------------------_
[\\\\\\\\\\\{*}:::<=================KINGSGUARD===================-       >
 |/~~~~~~~{o}----------------------------------------------------------~
           \<
            \<          
             \>

```

## Notes
This is a Windows userland rootkit. Given my infamous history with AppInit DLLs, I obviously decided to give it another go. 

In order to prevent loader lock and not slow the system down to a crawl, a PPID verification function was implemented inside of DllMain. Only processes at and below `explorer.exe` will successfully load this rootkit. 

The hooking library being used in MinHook. 
## Hooks
Kingsguard hooks the following functions for the purposes of evasion and protection:
- `NtQuerySystemInformation`
- `NtOpenProcess`
- `NtTerminateProcess`
- `NtEnumerateValueKey`
- `NtQueryValueKey`
- `NtSetValueKey`
- `NtDeleteValueKey`
- `NtOpenFile`
- `NtQueryDirectoryFile`

## Building / Usage
The project can be built with MinGW. Run `make` and find the compiled DLL as `kingsguard.dll`. Run `make 32bit` for a 32-bit version.

Options are set within `include/config.h`. 
- `DLL` -> DLL name to hide / remove from PEB
- `PROCESSA/W` -> Process name to hide / protect
- `KEY` -> Registry value name to hide / protect
- `PATH` -> File system path to protect
- `TAUNT` -> Set to `true` to modify all process names to `DLL` value
- `NOTERM` -> Set to `true` to prevent users from killing other processes

**IMPORTANT: all values in `config.h` MUST be lowercase.**

### Install
An install script is provided. It needs more testing and should be refined, but it works for now.

## Limitations 
One note on the explorer.exe loading. This is a limitation that can be easily bypassed by running under a service process. System integrity is more important to me.

Also, the hooks are not all encompassing. If a function is called with specific right not accounted for, it will bypass and call the original hook. One example of this is with NtOpenFile. The current function looks only for `DELETE` in the DesiredAccess mask, but there are other flags that can be used to delete a file like `FILE_SHARE_DELETE_ON_CLOSE`.

Another note on the PATH hook - it searches for a substring. For example, if it is set to `C:\Users`, any files at this directory and its respective subdirectories will be locked. Be careful.

Lastly, all processes seem to load via AppInit minus a few crucial ones like reg.exe. I recommend pairing this with `regprotect.ps1` as a support object.

## TODO
- Having a hook instead of just modifying the PEB to remove the kingsguard dll. Procexp is still able to see the loaded DLL as of this moment.