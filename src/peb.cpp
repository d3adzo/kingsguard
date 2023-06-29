#include "kingsguard.h"

// functions pulled from vx api - ty

mPPEB GetPeb(VOID)
{
#if defined(_WIN64)
	return (mPPEB)__readgsqword(0x60);
#elif define(_WIN32)
	return (mPPEB)__readfsdword(0x30);
#endif
}

VOID RemoveEntryList(LIST_ENTRY* Entry)
{
	if (Entry != NULL) {
		PLIST_ENTRY OldFlink;
		PLIST_ENTRY OldBlink;
		OldFlink = Entry->Flink;
		OldBlink = Entry->Blink;
		OldFlink->Blink = OldBlink;
		OldBlink->Flink = OldFlink;
		Entry->Flink = NULL;
		Entry->Blink = NULL;
	}
}

BOOL RemoveDllFromPebW(_In_ LPCWSTR lpModuleName) {
	mPPEB Peb = GetPeb();
	mPLDR_MODULE Module = NULL;

	PLIST_ENTRY Head = &Peb->LoaderData->InMemoryOrderModuleList;
	PLIST_ENTRY Next = Head->Flink;
	Module = (mPLDR_MODULE)((PBYTE)Next - 16);

	while (Next != Head)
	{
		Module = (mPLDR_MODULE)((PBYTE)Next - 16);
		if (Module->BaseDllName.Buffer != NULL)
		{
			if (CheckExistsW(std::wstring(Module->BaseDllName.Buffer), lpModuleName, false))
			{
				RemoveEntryList(&Module->InLoadOrderModuleList);
				RemoveEntryList(&Module->InInitializationOrderModuleList);
				RemoveEntryList(&Module->InMemoryOrderModuleList);
				RemoveEntryList(&Module->HashTableEntry);

				return TRUE;
			}
		}
		Next = Next->Flink;
	}

	return FALSE;
}