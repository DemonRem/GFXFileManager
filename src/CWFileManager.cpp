#include "CWFileManager.h"
#include "GFXInfo.h"
#include <limits>

#include "commandine.h"

// Shitty implemented internal funcs 
int check_file_exists(char *lpFileName, int arg);
int check_file_attributes(char *lpFileName, int arg);

int find_populate_result_entry(result_entry_t *result_entry);



CWFileManager::CWFileManager()
	: opened_files_ident(0) 
{
	populate_cmdline();
}

int CWFileManager::Mode() {
	return 2;
}

int CWFileManager::ConfigSet(int key, int value) {
	if (key == 2) 
		this->disallow_uppercase_filename = value != 0;

	return 0;
}

int CWFileManager::ConfigGet(int a2, int a3) {
	return 0;
}


int CWFileManager::CreateContainer(const char *filename, const char *password) {

	this->container_info = open_container_info_write(filename);

	return 1;
}


int CWFileManager::OpenContainer(const char *filename, const char* password, int mode) {

	this->container_info = open_container_info_write(filename);

	return 1;
}


int CWFileManager::IsOpen(void) {
	// hardcoded, always open
	return 1;
}

int CWFileManager::CloseAllFiles(void) {
	// fk, no ranged for loops in VS2010

	for (auto it = open_files.begin(); it != open_files.end(); it++) {
		this->Close(it->first);
	}
	
	return 1;
}

HMODULE CWFileManager::MainModuleHandle(void) {
	return this->hMainModule;
}

int CWFileManager::Function_9(int a2) {
	// harcoded, always -1
	return -1;
}

int CWFileManager::Delete(const char *filename) {
	return 0; // Deleting files is not supported by original CWFileManager!
}

int CWFileManager::Open(CJArchiveFm* fm, const char *filename, int access, int unknown) {
	fm->field_15 = 1;
	fm->pFileManager = this;
	fm->hFile = Open(filename, access, unknown);
	fm->is_write_mode = access == GENERIC_WRITE;

	if (fm->is_write_mode) {
		fm->field_20 = (int)&fm->field_28;
	} else {
		fm->field_20 = fm->field_24;
	}

	return fm->hFile =! -1;
}

int CWFileManager::Open(const char *filename, int dwDesiredAccess, int unknown) {
	char full_filename[520];

	// Assemble path
	strcpy_s(full_filename, sizeof(full_filename), this->current_dir);
	strcat_s(full_filename, sizeof(full_filename), filename);

	// Restrict uppercase if set
	if (this->disallow_uppercase_filename) {
		for (const char *f = filename; *f; f++) {
			if (*f >= 'A' && *f <= 'Z') {
				char output[260];
				sprintf(output, "FM File(%s)\n", full_filename);
				OutputDebugString(output);
				return -1;
			}
		}
	}

	// Look for empty directory names (LUL)
	if (full_filename[0]) {
		for (char *f = full_filename; *f; f++) {
			if (*f == '\\' && *(f+1) == '\\')
			{
				char output[260];
				sprintf(output, "FM File(%s)\n", full_filename);
				OutputDebugString(output);
				return -1;
			}
		}
	}

	int dwShareMode = 0;
	int dwCreationDistribution = 0;

	if (dwDesiredAccess == GENERIC_WRITE) {
		dwCreationDistribution = CREATE_ALWAYS;
	} else {
		dwCreationDistribution = OPEN_EXISTING;
		if (dwDesiredAccess == GENERIC_READ) {
			dwShareMode = FILE_SHARE_READ;
		}
	}

	HANDLE hFile = CreateFile(full_filename, dwDesiredAccess, dwShareMode, 0, dwCreationDistribution, FILE_ATTRIBUTE_ARCHIVE, 0);

	// Check if open file was successful
	if (hFile == INVALID_HANDLE_VALUE) {
		char error_buffer[256];
		sprintf(error_buffer, "FM File(%s)\n", full_filename);
		OutputDebugString(error_buffer);
		return -1;
	}

	int findex = GetNextFreeIndex();

	// TODO: GetNextFreeIndex will never return -1
	// I'm not even sure if it did in the original implementation ...
	if (findex == -1) {
		SHOW_ERROR("Could not open file", "Caption");
		return -1;
	}

	// Get Structure
	auto finfo = this->open_files[findex];

	// Populate Structure
	finfo.hFile;
	strcpy_s(finfo.filename, sizeof(finfo.filename), filename);

	return findex;
}

int CWFileManager::GetNextFreeIndex() {
	std::hash_map<int, OpenFileInfo>::iterator it;

	// Note the potential lockup once you hit INT_MAX+1 open files
	// @TODO: Doublecheck and if true, prevent this shit
	do {
		if (++this->opened_files_ident >= INT_MAX)
			this->opened_files_ident = 1;

		it = open_files.find(this->opened_files_ident);

	} while (it != open_files.end());

	OpenFileInfo info;
	info.field_0 = this->opened_files_ident; // guessed

	// Copy this thing into the map
	// Also guessed, not sure what this looked like before.
	this->open_files[this->opened_files_ident] = info;

	return this->opened_files_ident;
}

int CWFileManager::Function_12(void) {
	return -1;
}

int CWFileManager::Function_13(void) {
	return 0;
}

int CWFileManager::Create(const char* filename, int unknown) {
	if (this->CreateDirectoryRecursive(filename)) {
		return this->Open(filename, GENERIC_READ, unknown);
	}
	
	return -1;
}

int CWFileManager::Create(CJArchiveFm * fm, const char * filename, int unknown) {
	fm->field_15 = 1;
	fm->is_write_mode = 1;
	fm->pFileManager = this;
	
	fm->hFile = this->Create(filename, unknown);;

	if (fm->is_write_mode) {
		fm->field_20 = fm->field_24;
	} else {
		fm->field_20 = fm->field_28;
	}

	return fm->hFile != -1;
}

// This method has a bug, see #16
bool CWFileManager::CreateDirectoryRecursive(const char* filename) {
	char buffer[512] = {0};
	char *token;
	
	char previous_dir[512] = {0};

	GetCurrentDirectory(sizeof(previous_dir), previous_dir);

	// Original path for finding the last path separator
	std::string original(filename);	
	size_t pos = original.find_last_of('\\');

	// Copy path without filename to new buffer	
	memcpy(buffer, original.c_str(), pos);

	// Start tokenizing
	token = strtok(buffer, "\\");

	// Growing path variable
	std::string fullpath = "";

	while (token != NULL) {
		// Add current folder to path
		fullpath += token;

		// Create entire path
		CreateDirectory(fullpath.c_str(), 0);

		// Append separator
		fullpath += "\\";

		// Get next token
		token = strtok(NULL, "\\");
	}

	// Check if directories were created
	int result = 1;
	if (!SetCurrentDirectory(fullpath.c_str())) {
		result = 0;
	}

	// Reset directory to previous
	SetCurrentDirectory(previous_dir);

	return result;
}

int CWFileManager::Close(int hFile) {
	auto file = this->open_files.find(hFile);

	if (file == this->open_files.end()) {
		// File Handle is invalid
		// 유효하지 않은 파일 객체 이다.
		SHOW_ERROR("File Handle is invalid", "Error during Close");
		return 0;
	}

	HANDLE file_handle = file->second.hFile;

	this->open_files.erase(file);
	this->container_info->number_of_open_files--;
	return ::CloseHandle(file_handle);
}


int CWFileManager::Read(int hFile, char* lpBuffer, int nNumberOfBytesToRead, unsigned long *lpNumberOfBytesRead) {
	auto file = this->open_files.find(hFile);

	if (file == this->open_files.end()) {
		// File Handle is invalid
		// 유효하지 않은 파일 객체 이다.
		SHOW_ERROR("File Handle is invalid", "Error during Read");
		return 0;
	}

	BOOL result = ::ReadFile(file->second.hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, 0);

	this->container_info->number_of_bytes_processed_total += *lpNumberOfBytesRead;

	return result;
}

int CWFileManager::Write(int hFile, const char* lpBuffer, int nNumberOfBytesToWrite, unsigned long *lpNumberOfBytesWritten) {
	auto file = this->open_files.find(hFile);

	if (file == this->open_files.end()) {
		// File Handle is invalid
		// 유효하지 않은 파일 객체 이다.
		SHOW_ERROR("File Handle is invalid", "Error during Write");
		return 0;
	}

	BOOL result = ::WriteFile(file->second.hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, 0);

	this->container_info->number_of_bytes_processed_total += *lpNumberOfBytesWritten;

	return result;
}


char* CWFileManager::CmdLinePath(void) {
	return get_cmdline_path();
}

char* CWFileManager::CmdLineExe(void) {
	return get_cmdline_exe();
}


//
// File Information
//

int CWFileManager::FileNameFromHandle(int hFile, char* dst, size_t count) {
	auto file = this->open_files.find(hFile);

	if (file == this->open_files.end()) {
		// File Handle is invalid
		// 유효하지 않은 파일 객체 이다.
		SHOW_ERROR("File Handle is invalid", "Error during FileNameFromHandle");
		return 0;
	}

	int len = strlen(file->second.filename);

	if (len >= count)
		return 0;

	strcpy_s(dst, count, file->second.filename);

	return 1;
}

int CWFileManager::GetFileSize(int hFile, LPDWORD lpFileSizeHigh) {
	auto file = this->open_files.find(hFile);

	if (file == this->open_files.end()) {
		// File Handle is invalid
		// 유효하지 않은 파일 객체 이다.
		SHOW_ERROR("File Handle is invalid", "Error during GetFileSize");
		return 0;
	}

	return ::GetFileSize(file->second.hFile, lpFileSizeHigh);
}

BOOL CWFileManager::GetFileTime(int hFile, LPFILETIME lpCreationTime, LPFILETIME lpLastWriteTime) {
	auto file = this->open_files.find(hFile);

	if (file == this->open_files.end()) {
		// File Handle is invalid
		// 유효하지 않은 파일 객체 이다.
		SHOW_ERROR("File Handle is invalid", "Error during GetFileTime");
		return 0;
	}

	return ::GetFileTime(file->second.hFile, lpCreationTime, 0, lpLastWriteTime);
}

BOOL CWFileManager::SetFileTime(int hFile, LPFILETIME lpCreationTime, LPFILETIME lpLastWriteTime) {
	auto file = this->open_files.find(hFile);

	if (file == this->open_files.end()) {
		// File Handle is invalid
		// 유효하지 않은 파일 객체 이다.
		SHOW_ERROR("File Handle is invalid", "Error during SetFileTime");
		return 0;
	}

	return ::SetFileTime(file->second.hFile, lpCreationTime, 0, lpLastWriteTime);
}

int CWFileManager::Seek(int hFile, LONG lDistanceToMove, DWORD dwMoveMethod) {

	auto file = this->open_files.find(hFile);

	if (file == this->open_files.end()) {
		// File Handle is invalid
		// 유효하지 않은 파일 객체 이다.
		SHOW_ERROR("File Handle is invalid", "Error during Seek");
		return 0;
	}

	return ::SetFilePointer(file->second.hFile, lDistanceToMove, 0, dwMoveMethod);
}

int CWFileManager::Lock(int a) {
	return 0;
}

int CWFileManager::Unlock() {
	return 0;
}

int CWFileManager::Function_50(int a) {
	return 0;
}

int CWFileManager::UpdateCurrentDirectory() {
	return 0;
}

int CWFileManager::FileExists(char *filename, int a3)
{
	char fullpath[520]; // [sp+4h] [bp-20Ch]@1

	strcpy_s(fullpath, 0x208u, this->current_dir);
	strcat_s(fullpath, 0x208u, filename);

	return check_file_exists(fullpath, a3);
}

int check_file_exists(char *lpFileName, int arg) {
	return -(check_file_attributes(lpFileName, arg) != 0);
}

int check_file_attributes(char *lpFileName, int arg) {
	DWORD attrib;
	DWORD err;

	if ( !lpFileName || arg & 0xFFFFFFF9 )
	{
		*__doserrno() = 0;
		*_errno() = 22;
		// _invalid_parameter(0, 0, 0, 0, 0);
		return 22;
	}
	attrib = GetFileAttributesA(lpFileName);
	if ( attrib == INVALID_FILE_ATTRIBUTES )
	{
		err = GetLastError();
		// _dosmaperr(err);
		return *_errno();
	}
	if ( !(attrib & FILE_ATTRIBUTE_DIRECTORY) && (attrib & 1) && (arg & 2) )
	{
		*__doserrno() = 5;
		*_errno() = 13;
		return *_errno();
	}
	return 0;
}

int CWFileManager::ImportDirectory(const char *srcdir, const char *dstdir, const char *directory_name, bool create_target_dir) {
	return 0;
}

int CWFileManager::ImportFile(const char *srcdir, const char *dstdir, const char *filename, bool create_target_dir){
	return 0;
}

int CWFileManager::ExportDirectory(const char *srcdir, const char *dstdir, const char *directory_name, bool create_target_dir){
	return 0;
}

int CWFileManager::ExportFile(const char *srcdir, const char *dstdir, const char *filename, bool create_target_dir){
	return 0;
}

void CWFileManager::RegisterErrorHandler(error_handler_t callback) {
	this->error_handler = error_handler;
}

HWND CWFileManager::GetHwnd(void) {
	return this->hwnd;
}

void CWFileManager::SetHwnd(HWND hwnd) {
	this->hwnd = hwnd;
}


int find_populate_result_entry(result_entry_t *result_entry)
{
	strcpy_s(result_entry->filename, 89u, result_entry->find_data.cFileName);


	result_entry->file_time.dwLowDateTime = result_entry->find_data.ftCreationTime.dwHighDateTime;
	result_entry->file_time.dwHighDateTime = result_entry->find_data.ftLastWriteTime.dwLowDateTime;

	result_entry->LowDateTime = result_entry->find_data.ftCreationTime.dwLowDateTime;
	result_entry->HighDateTime = result_entry->find_data.ftLastWriteTime.dwHighDateTime;

	result_entry->size =  result_entry->find_data.nFileSizeLow;


	result_entry->field_18 = 0;
	result_entry->field_1C = 0;
	result_entry->field_10 = 0;
	result_entry->field_14 = 0;
	result_entry->type = ((result_entry->find_data.dwFileAttributes & 0x10) != 16) + 1;
	return 0;
}


struct searchresult_t * CWFileManager::FindFirstFile(struct searchresult_t *search_result, const CHAR *lpFileName, struct result_entry_t *result_entry)
{
	HANDLE hFind;
	struct searchresult_t *result;
	WIN32_FIND_DATA find_data;

	::SetCurrentDirectory(this->current_dir);

	hFind = ::FindFirstFile(lpFileName, &result_entry->find_data);
	search_result->hFind = hFind;
	if ( hFind == INVALID_HANDLE_VALUE )
	{
		search_result->success = 0;
		result = search_result;
	}
	else
	{
		search_result->success = 1;
		memcpy(&find_data, &result_entry->find_data, sizeof(find_data));
		find_populate_result_entry(result_entry);
		result = search_result;
	}
	return result;
}

int CWFileManager::FindNextFile(struct searchresult_t *a2, struct result_entry_t *a1)
{
	int result; // eax@2
	WIN32_FIND_DATA v4; // [sp-140h] [bp-14Ch]@2

	if ( ::FindNextFile(a2->hFind, &(a1->find_data)) )
	{
		memcpy(&v4, &a1->find_data, sizeof(v4));
		find_populate_result_entry(a1);
		result = 1;
	}
	else
	{
		a2->success = 0;
		result = 0;
	}
	return result;
}

int CWFileManager::FindClose(struct searchresult_t *a2)
{
	::FindClose(a2->hFind);
	return 1;
}

int CWFileManager::GetVirtualPath(char *Dst)
{
	strcpy_s(Dst, sizeof(this->initial_path), this->initial_path);
	return 1;
}

int CWFileManager::SetVirtualPath(const char *Src)
{
	strcpy_s(this->initial_path, sizeof(this->initial_path), Src);
	strcpy_s(this->current_dir, sizeof(this->current_dir), Src);
	return 1;
}


int CWFileManager::GetDirectoryName(rsize_t SizeInBytes, char *outname)
{
	strcpy_s(outname, SizeInBytes, this->current_dir);
	return strlen(outname);
}

BOOL CWFileManager::DirectoryCreate(const CHAR *lpPathName)
{
  SetCurrentDirectory(this->current_dir);
  return CreateDirectory(lpPathName, 0);
}

BOOL CWFileManager::DirectoryRemove(LPCSTR lpPathName)
{
  SetCurrentDirectory(this->current_dir);
  return RemoveDirectory(lpPathName);
}

shit_t* CWFileManager::getShit(struct shit_t *a2)
{
  a2->a = 0;
  a2->b = 0;
  return a2;
}

int CWFileManager::setShit(int a2, int a3)
{
  return 0;
}

int CWFileManager::CloseContainer()
{
  open_container_info_delete(this->container_info);
  this->container_info = 0;                     // // doesnt this set the inuse flag to 0 ???
  return 1;
}
