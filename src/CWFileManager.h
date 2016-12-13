#pragma once

#include "IFileManager.h"
#include "GFXInfo.h"

#include <hash_map>

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

	struct OpenFileInfo {
		int field_0;
		HANDLE hFile;
		char filename[260];
	};

	char initial_path[260]; // 0x4
	char current_dir[260]; // 0x108
	gfxinfo_t *container_info; // 0x20C
		
	// consider switching this to unordered_map (wasnt present in VC80)
	std::hash_map<int, OpenFileInfo> open_files; // 0x210

	int opened_files_ident;
	char disallow_uppercase_filename; // 0x23C

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

	
	virtual int Create(CJArchiveFm * fm, const char * filename, int unknown); //
	virtual int Create(const char* filename, int unknown); //

	
	//
	// File Information
	//

	virtual int FileNameFromHandle(int hFile, char* dst, size_t count);
	virtual int GetFileSize(int hFile, LPDWORD lpFileSizeHigh); //
public:
	CWFileManager();

private:
	int GetNextFreeIndex();
	bool CreateDirectoryRecursive(const char* filename);

};
