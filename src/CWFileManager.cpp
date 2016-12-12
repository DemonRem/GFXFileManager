#include "CWFileManager.h"
#include "GFXInfo.h"

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

	 if (hFile == INVALID_HANDLE_VALUE) {

		 if (this->error_handler) {
			 if (!this->error_handler((HWND)1, "파일 열기 실패.", "false")) {
				 MessageBox(this->hwnd, "파일 열기 실패.", 
			 }
		 } else {

		 }



	 }

 }


int CWFileManager::Function_12(void) {
	return -1;
}

int CWFileManager::Function_13(void) {
	return 0;
}
