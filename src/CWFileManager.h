#pragma once

#include "IFileManager.h"
#include "GFXInfo.h"

class CWFileManager : public IFileManager {
private:

	char initial_path[260]; // 0x4
	char current_dir[260]; // 0x108
	gfxinfo_t *container_info; // 0x20C

	error_handler_t error_handler; // 0x268
	HWND hwnd; // 0x026C

public:
	virtual int Mode();
	virtual int Function_1(int a2, int a3);
	virtual int Function_2(int a2, int a3);
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
