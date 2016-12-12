#include "CWFileManager.h"
#include "GFXInfo.h"
#include <limits>


CWFileManager::CWFileManager()
	: opened_files_ident(0) 
{

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


int Create(CJArchiveFm * fm, const char * filename, int unknown) {

}

int CWFileManager::Create(const char* filename, int unknown) {
	if (this->CreateDirectoryRecursive(filename)) {
		return this->Open(filename, GENERIC_READ, unknown);
	}
	
	return -1;
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

