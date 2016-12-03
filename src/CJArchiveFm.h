#pragma once

#include "IFileManager.h"

#pragma pack(push, 1)
class CJArchiveFm
{
public:
	virtual ~CJArchiveFm() {
	};

	IFileManager *pFileManager; //0x0004 ptr to object

	int hFile; //0x0008 filehandle

	int field_C;
	int field_10;

	unsigned char is_write_mode; //0x0014 
	unsigned char field_15; //0x0015 

	char pad_0x0016[0xA]; //0x0016
	int field_20; //0x0020
	int field_24; //0x0024
	int field_28; //0x0028

};//Size=0x002C
#pragma pack(pop)
