#pragma once
#include <Windows.h>
#include <sysinfoapi.h>
#include "vector.h"
#include "includes/modules/spoofer/call_stack_spoofer.hpp"
#include "includes/modules/security/lazy_importer.h"
#pragma warning(disable : 4996)
//#include "includes/modules/modules.hpp"
//#define ALIGN_DOWN_BY(length, alignment) \
//    ((ULONG_PTR)(length) & ~((ULONG_PTR)(alignment) - 1))
//
//#define ALIGN_DOWN_POINTER_BY(address, alignment) \
//    ((PVOID)((ULONG_PTR)(address) & ~((ULONG_PTR)(alignment) - 1)))
//
//
//
//bool IsValidPtrnew(void* pointer)
//{
//	if (!pointer)
//		return false;
//
//	if (pointer < (void*)0xFFFF)
//		return false;
//
//	if (pointer > (void*)0x7FFFFFFFFFFF)
//		return false;
//
//	return true;
//}
//
//
//class MemoryManager {
//private:
//	SYSTEM_INFO sysInfo{};
//	BOOL InfoInit = FALSE;
//public:
//	void InitialInfo() {
//		safe_call(GetSystemInfo)(&sysInfo);
//		InfoInit = TRUE;
//	}
//
//	BOOL PTRCheck(LPCVOID lp, UINT_PTR ucb)
//	{
//		if (!Memory->isValidPointernew((void*)lp))
//			return 0;
//
//
//		if (InfoInit == FALSE)
//			InitialInfo();
//		ULONG PageSize;
//		BOOLEAN Result = FALSE;
//		volatile CHAR* Current;
//		PCHAR Last;
//
//		if (!ucb)
//			return FALSE;
//		if (!lp) 
//		return TRUE;
//		PageSize = sysInfo.dwPageSize;
//
//
//		Current = (volatile CHAR*)lp;
//
//		if (!Memory->isValidPointernew((void*)Current))
//			return 0;
//
//		Last = (PCHAR)((ULONG_PTR)lp + ucb - 1);
//
//		if (Last < Current) return TRUE;
//
//		__try
//		{
//			*Current;
//
//			Current = (volatile CHAR*)ALIGN_DOWN_POINTER_BY(Current, PageSize);
//			Last = (PCHAR)ALIGN_DOWN_POINTER_BY(Last, PageSize);
//
//			while (Current != Last)
//			{
//				Current += PageSize;
//				*Current;
//			}
//		}
//		__except (EXCEPTION_EXECUTE_HANDLER)
//		{
//			Result = TRUE;
//		}
//		return Result;
//	}
//
//	template <typename T>
//	inline T RVM(DWORD_PTR Address) {
//		if (PTRCheck(reinterpret_cast<PVOID>(Address), sizeof(T))) {
//			return T{};
//		}
//		return *(T*)Address;
//	}
//	template <typename T>
//	BOOL WVM(DWORD_PTR Address, T Value) {
//		if (PTRCheck(reinterpret_cast<PVOID>(Address), sizeof(T))) {
//			return 0;
//		}
//		*(T*)Address = Value;
//	}
//
//	
//};

class MemoryManager {
public:
	bool isValidPointer(LPVOID address, UINT_PTR length = 8)
	{
		SPOOF_FUNC;
		// 首先检查指针是否为空以及是否在合理的范围内
		if (!address || address < (void*)0xFFFF || address >(void*)0x7FFFFFFFFFFF)
			return false;

		// 使用 IsBadWritePtr 检查指针的写入权限
		return !safe_call(IsBadWritePtr)(address, length);
	}
	template <typename T>
	inline T RVM(DWORD_PTR Address) {
		SPOOF_FUNC;

		if (!isValidPointer(reinterpret_cast<PVOID>(Address), sizeof(T))) {
			return T{};
		}
		return *(T*)Address;
	}
	template <typename T>
	BOOL WVM(DWORD_PTR Address, T Value) {
		SPOOF_FUNC;

		if (!isValidPointer(reinterpret_cast<PVOID>(Address), sizeof(T))) {
			return 0;
		}
		*(T*)Address = Value;
	}


};
static MemoryManager* GameMemory = new MemoryManager;