
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
- `NtOpenFile`
- `NtOpenProcess`
- `NtTerminateProcess`
- `NtEnumerateValueKey`
- `NtQueryValueKey`
- `NtDeleteValueKey`

## Building / Usage
The project can be built with MinGW. Run `make` and find the compiled DLL in `bin/kingsguard.dll`.

Options are set within `include/config.h`. 
- `DLL` -> DLL name to remove from PEB
- `PROCESSA/W` -> Process word to hide / protect
- `KEY` -> Registry value name to hide / protect
- `PATH` -> File system path to protect
- `TAUNT` -> Set to `true` to modify all process names to `DLL` value

**IMPORTANT: all values in `config.h` MUST be lowercase.**

### Install
An install script is provided. It needs more testing and should be refined, but it works for now.

## Limitations 
One note on the explorer.exe loading. This is a limitation that can be easily bypassed by running under a service process or by killing explorer and restarting it. System integrity is more important to me.

Also, the hooks are not all encompassing. If a function is called with specific right not accounted for, it will bypass and call the original hook. One example of this is with NtOpenFile. The current function looks only for `DELETE` in the DesiredAccess mask, but there are other flags that can be used to delete a file like `FILE_SHARE_DELETE_ON_CLOSE`.

Lastly, all processes seem to load via AppInit except reg.exe. I recommend pairing this with `regprotect.ps1` as a support object.

## TODO
- More testing on the PEB modification stability. 
- Having a hook instead of just modifying the PEB to remove the kingsguard dll. Procexp is still able to see the loaded DLL as of this moment.