#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>

#include "CPFileManager.h"
//#include "CWFileManager.h"
#include "debug.h"

HMODULE hInstance;

#define EXPORT extern "C" __declspec(dllexport) 

#define TARGET_VERSION 0x1007
#define MODE_ARCHIVE 1
#define MODE_FILESYSTEM 2

extern FILE *dbgfile;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulReason, LPVOID lpReserved) {

	UNREFERENCED_PARAMETER(lpReserved); // Suppress warning, lol

	if (ulReason == DLL_PROCESS_ATTACH) {
		hInstance = hModule;

		AllocConsole();
		freopen("CONOUT$", "w", stdout);

		dbgfile = fopen("gfxlog.txt", "w");

		DisableThreadLibraryCalls(hModule);
	} else if (ulReason == DLL_PROCESS_DETACH) {

		fclose(dbgfile);

	}

	return TRUE;
}

EXPORT int __stdcall  GFXDllCreateObject(int mode, IFileManager** object, int version) {
	char message[100];

	debug(DEBUG_OBJECT, "GFXDllCreateObject(%08x, %08x, %04x)\n", mode, object, version);
	//MessageBox(0, "CreateObject called", "", MB_OK);

	if (version != TARGET_VERSION) {
		sprintf(message, "Dll Version(%x)\nNecessary Version (%x)", version, TARGET_VERSION);
		MessageBox(0, message, "Invalid Version(GFXFileManager.dll)", MB_OK|MB_APPLMODAL);
		return 0;
	}

	if (mode == MODE_ARCHIVE) {
		*object = new CPFileManager();
	} else if (mode == MODE_FILESYSTEM) {
		*object = new CPFileManager();
	} else {
		*object = 0;
	}

	return 0;
}

EXPORT void __stdcall GFXDllReleaseObject(IFileManager* object) {
	debug(DEBUG_OBJECT, "GFXDllReleaseObject(%08x)\n", object);
	delete object;
}

