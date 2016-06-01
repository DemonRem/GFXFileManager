#pragma once 

#include <Windows.h>
#include <stdio.h>

#include <string>

#include "shit_t.h"
#include "CJArchiveFm.h"
#include "searchresult.h"
#include "result_entry_t.h"

#define FM_VERSION 0x1007


typedef BOOL (*error_handler_t)(void);



class IFileManager
{
public:
	virtual int get_mode(void) = 0; //returns the container-mode (1 for CP, 2 for CW)
	virtual int Function_1(int, int) = 0; //
	virtual int Function_2(int, int) = 0; //

	//
	// Container
	// 

	// Create a new container
	// Parameter:
	// - filename: filename of the container
	// - password: password for accessing the new container
	virtual int CreateContainer(const char *filename,  const char *password) = 0;

	// Open an existing container
	// Parameter:
	// - filename: filename of the container
	// - password: password required for accessing the container
	// - mode: unknown, maybe for read and write access
	virtual int OpenContainer(const char *filename, const char* password, int mode) = 0;

	// Close the current container
	// No Parameter
	virtual int CloseContainer(void) = 0; //


	// Returns 1, if this instance has opened a container
	virtual int IsOpen(void) = 0; //



	virtual int Function_7(void) = 0; //Similar in both implementations

	// Returns the MainModule-handle
	virtual int getMainModule(void) = 0;


	virtual int Function_9(int) = 0; //CWFileManager returns -1


	//
	// Files
	//

	// Open a file inside the container using the CJArchiveFm-class
	// Parameter:
	// - fm: A valid pointer to the CJArchiveFm-class
	// - filename: filename, relative to current dir or absolute path inside archive
	// - access: 0 for open-existing, 0x80000000 for open and share_read, 0x40000000 for create_always
	// - unknown: not used for original CWFileManager
	virtual int Open2(CJArchiveFm* fm, std::string *filename, int access, int unknown) = 0;

	// Open a file inside the container using a path
	// Parameter:
	// - filename: filename, relative to current dir or absolute path inside archive
	// - access: 0 for open-existing, 0x80000000 for open and share_read, 0x40000000 for create_always
	// - unknown: not used for original CWFileManager
	// Return:
	// Handle of opened file (can be any number or pointer) or -1 if opening is was unsuccessful
	virtual int Open(std::string *filename, int access, int unknown) = 0; //

	virtual int Open2_Old(CJArchiveFm* fm, const char *filename, int access, int unknown) = 0; //return -1
	virtual int Open_Old(const char *filename, int access, int) = 0; //return 0
	virtual int Function_14(int, int, int) = 0; //
	virtual int Function_15(char* fullpath, int) = 0; //

	// Delete a file by name
	// Parameter:
	// - filename: name of file to delete
	virtual int Delete(char *filename) = 0; //

	// Close file by handle
	// Parameter:
	// hFile: Any handle or pointer identifiying this file
	virtual int Close(int hFile) = 0; //

	// Read a number of bytes from file
	// Parameter:
	// hFile: Any handle or pointer identifiying this file
	// lpBuffer: pointer to reserved memory for read operation
	// nNumberOfBytesToWrite: size of lpBuffer
	// lpNumberOfBytesWritten: pointer to memory, will contain the number of bytes read from the file
	virtual int Read(int hFile, char* lpBuffer, int nNumberOfBytesToWrite, unsigned long *lpNumberOfBytesWritten) = 0;

	// Write a number of bytes to file
	// Parameter:
	// hFile: Any handle or pointer identifiying this file
	// lpBuffer: pointer to reserved memory for read operation
	// nNumberOfBytesToWrite: size of lpBuffer
	// lpNumberOfBytesWritten: pointer to memory, will contain the number of bytes written to the file
	virtual int Write(int hFile, char* lpBuffer, int nNumberOfBytesToWrite, unsigned long *lpNumberOfBytesWritten) = 0;




	// Get the full path of the executable
	virtual char* getCmdLine_Path(void) = 0;

	// Get raw commandline args
	virtual char* getCmdLine_Args(void) = 0; //

	// Unknown function that gets two variables
	virtual void getShit(shit_t* shit) = 0; //get shit

	// Unknown function that sets two variables
	virtual int setShit(int a, int b) = 0; //set shit


	//
	// Directory Management
	//

	// Create a new directory in the container
	// Parameter:
	// - name: name of the directory
	virtual int CreateDirectory(const char* name) = 0;

	// Delete a directory in the container
	// Parameter:
	// - name: name of the directory
	virtual int RemoveDirectory(const char* name) = 0;


	virtual bool ResetDirectory(void) = 0; //
	virtual bool ChangeDirectory(char* dirname) = 0; //
	virtual int GetDirectoryName(size_t buffersize, char* Dst) = 0; //


	virtual int SetVirtualPath(char *path) = 0; //set root
	virtual int GetVirtualPath(char* dest) = 0; //similar on both impl


	//
	// Searching
	//

	// Start a new search for files
	// Parameter:
	// - search: result structure representing a handle for the search
	// - pattern: matching pattern for a list of files
	// - entry: the output-structure for the first entry of the resulting list of files
	virtual searchresult_t* FindFirstFile(searchresult_t* search, char* pattern, result_entry_t* entry) = 0; //Find First File

	// Get the next file entry in the search result list
	// Parameter:
	// - search: result structure representing a handle for the search
	// - entry: the output-structure for the next entry of the resulting list of files
	virtual int FindNextFile(searchresult_t* search, result_entry_t* entry) = 0;

	// Free the search result
	// Parameter:
	// - search: result structure representing a handle for the search
	virtual int FindClose(searchresult_t* search) = 0;


	//
	// File Information
	//

	virtual int FileNameFromHandle(int hFile, char* dst, size_t count) = 0; //GetFileName
	virtual int GetFileSize(int hFile, LPDWORD lpFileSizeHigh) = 0; //
	virtual BOOL GetFileTime(int hFile, LPFILETIME lpCreationTime, LPFILETIME lpLastWriteTime) = 0; //
	virtual BOOL SetFileTime(int hFile, LPFILETIME lpCreationTime, LPFILETIME lpLastWriteTime) = 0; //
	virtual int Seek(int hFile, LONG lDistanceToMove, DWORD dwMoveMethod) = 0; //


	//
	// Others
	//

	virtual int get_hwnd(void) = 0; //
	virtual void set_hwnd(int) = 0; //
	virtual void set_error_handler(error_handler_t callback) = 0; //sets a callback

	virtual int Function_42(int, int, int, int) = 0; //Search file on disk (historic code?)
	virtual int ImportFile(char *srcdir, char *dstdir, char *filename, int d) = 0; //open file(s)
	virtual int Function_44(int, int, int, int) = 0; //create directories on disk
	virtual int Function_45(int, int, int, int) = 0; //Open File(s) and write

	// Returns: 0 on found, -1 on not found
	virtual int file_exists(char* name, int flags) = 0; //

	// Shows an open file dialog
	virtual int ShowDialog(int*);
	virtual int Function_48(int a, int b, int c); //
	virtual int Function_49(void) = 0; //
	virtual int Function_50(int) = 0; //returns zero in both impl.

	// get the version of this file manager
	virtual int getVersion(void);

	//prompt error if version mismatch 
	virtual int checkVersion(int version);

	virtual int Function53(int) = 0; //
	virtual int Function54() = 0; //
	virtual ~IFileManager(void) {
	};
};//Size=0x0004
