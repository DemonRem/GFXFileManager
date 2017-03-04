#pragma once

#include "IFileManager.h"
#include "GFXInfo.h"

#include <hash_map>

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

	HMODULE hMainModule; // 0x21C

	int opened_files_ident; // 0x238
	char disallow_uppercase_filename; // 0x23C

	error_handler_t error_handler; // 0x268
	HWND hwnd; // 0x026C

public:
	virtual int Mode();
	virtual int ConfigSet(int a2, int a3);
	virtual int ConfigGet(int a2, int a3);


	//
	// Container
	//

	virtual int CreateContainer(const char *filename,  const char *password);
	virtual int OpenContainer(const char *filename, const char* password, int mode);
	virtual int CloseContainer(void);

	virtual int IsOpen(void) = 0; //

	virtual int CloseAllFiles(void) = 0; //Similar in both implementations

	virtual HMODULE MainModuleHandle(void);

	virtual int Function_9(int) = 0;

	//
	// Files
	//

	virtual int Open(const char *filename, int access, int unknown);
	virtual int Open(CJArchiveFm* fm, const char *filename, int access, int unknown);

	virtual int Function_12(void); //return -1
	virtual int Function_13(void); //return 0

	virtual int Create(const char* filename, int unknown); //
	virtual int Create(CJArchiveFm * fm, const char * filename, int unknown); //

	virtual int Delete (const char *filename);
	virtual int Close(int hFile); //

	virtual int Read(int hFile, char* lpBuffer, int nNumberOfBytesToWrite, unsigned long *lpNumberOfBytesWritten);
	virtual int Write(int hFile, const char* lpBuffer, int nNumberOfBytesToWrite, unsigned long *lpNumberOfBytesWritten);

		
	virtual char* CmdLinePath(void);
	virtual char* CmdLineExe(void);

	virtual shit_t* getShit(shit_t* shit);
	virtual int setShit(int a, int b);


	virtual int DirectoryCreate(const char* name);
	virtual int DirectoryRemove(const char* name);


	virtual bool ResetDirectory(void);
	virtual bool ChangeDirectory(const char* dirname);
	virtual int GetDirectoryName(size_t buffersize, char* Dst);
	
	virtual int SetVirtualPath(const char *path);
	virtual int GetVirtualPath(char *dest);


	virtual searchresult_t* FindFirstFile(searchresult_t* search, const char* pattern, result_entry_t* entry);
	virtual int FindNextFile(searchresult_t* search, result_entry_t* entry);
	virtual int FindClose(searchresult_t* search);

	//
	// File Information
	//

	virtual int FileNameFromHandle(int hFile, char* dst, size_t count);
	virtual int GetFileSize(int hFile, LPDWORD lpFileSizeHigh); //
	virtual BOOL GetFileTime(int hFile, LPFILETIME lpCreationTime, LPFILETIME lpLastWriteTime); //
	virtual BOOL SetFileTime(int hFile, LPFILETIME lpCreationTime, LPFILETIME lpLastWriteTime); //

	virtual int Seek(int hFile, LONG lDistanceToMove, DWORD dwMoveMethod); //


	virtual HWND GetHwnd(void);
	virtual void SetHwnd(HWND);
	virtual void RegisterErrorHandler(error_handler_t callback);


	virtual int ImportDirectory(const char *srcdir, const char *dstdir, const char *directory_name, bool create_target_dir);
	virtual int ImportFile(const char *srcdir, const char *dstdir, const char *filename, bool create_target_dir);
	virtual int ExportDirectory(const char *srcdir, const char *dstdir, const char *directory_name, bool create_target_dir);
	virtual int ExportFile(const char *srcdir, const char *dstdir, const char *filename, bool create_target_dir); // create_target_dir is unused

	virtual int FileExists(char* name, int flags); //


	virtual int UpdateCurrentDirectory(void); //
	virtual int Function_50(int); // returns zero in both impl.


	virtual int Lock(int); //
	virtual int Unlock(); //

public:
	CWFileManager();

private:
	int GetNextFreeIndex();
	bool CreateDirectoryRecursive(const char* filename);

	bool ChangeDirTo(const char*);

};
