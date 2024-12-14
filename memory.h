#pragma once
#include "globals.h"
#include "offsets.h"

//#include "includes/modules/modules.hpp"

static __forceinline bool ZwCopyMemory(uint64_t address, PVOID buffer, uint64_t size, BOOL write = false)
{
	SPOOF_FUNC;
	if (!GameMemory->isValidPointer((void*)address))
		return false;
	if (write)
	{
		safe_call(memcpy)((void*)address, buffer, size);
	}
	else
	{
		safe_call(memcpy)(buffer, (void*)address, size);
	}

	return true;
};




//static std::string ReadString(uint64_t address, size_t size)
//{
//	if (!address || size > 1024)
//		return "";
//
//	char string[1024] = "";
//	return ZwCopyMemory(address, string, size, false) ? std::string(string) : "";
//};
//
//std::string ReadArmaString(uint64_t address)
//{
//	int length = GameMemory->RVM<int>(address + OFF_LENGTH);
//
//	std::string text = ReadString(address + OFF_TEXT, length);
//
//	return text.c_str();
//}

static __forceinline std::string ReadArmaString(uint64_t address)
{
	int length = GameMemory->RVM<int>(address + OFF_LENGTH);
	if (!(address + OFF_TEXT) || length > 1024)
		return "";
	char string[1024] = "";
	std::string text;
	text = ZwCopyMemory(address + OFF_TEXT, string, length, false) ? std::string(string) : "";
	return text;
}