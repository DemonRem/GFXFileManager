#pragma once

#include "IFileManager.h"
#include "GFXInfo.h"

#define SHOW_ERROR(msg, caption) \
	do { \
		if (error_handler) { \
			if (!error_handler((HWND)1, msg, "false")) \
				MessageBoxA(hwnd, msg, caption, MB_OK); \
		} else { \
			MessageBoxA(hwnd, msg, caption, MB_OK); \
		} \
	} \
	while (0)


class CWFileManager : public IFileManager {
private:

	char initial_path[260]; // 0x4
	char current_dir[260]; // 0x108
	gfxinfo_t *container_info; // 0x20C

	error_handler_t error_handler; // 0x268
	HWND hwnd; // 0x026C

public:
	virtual int Mode();
	virtual int ConfigSet(int a2, int a3);
	virtual int ConfigGet(int a2, int a3);
	virtual int CreateContainer(const char *filename,  const char *password);
	virtual int OpenContainer(const char *filename, const char* password, int mode);

	virtual int IsOpen(void) = 0; //

	
	virtual int CloseAllFiles(void) = 0; //Similar in both implementations

	virtual int Function_9(int) = 0;

	virtual int Delete (const char *filename);

	virtual int Open(CJArchiveFm* fm, const char *filename, int access, int unknown);
	virtual int Open(const char *filename, int access, int unknown);

	virtual int Function_12(void); //return -1
	virtual int Function_13(void); //return 0

};
