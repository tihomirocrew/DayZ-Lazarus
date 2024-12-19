#include "lazy_importer.hpp"
#include "call_stack_spoofer.hpp"
//#include "includes/modules/windows/windows_api.h"
#include "overlay.h"
#include <iostream>
#include "safecall.h"



//#include "includes/modules/modules.hpp"
//inline uintptr_t GetGameBase() {
//	return *(uintptr_t*)(__readgsqword(0x60) + 0x10);
//}



//#define DebugBuildMode


HMODULE MyModule = {  };

void MainWhile() {
	SPOOF_FUNC;
	globals.Base = *(uintptr_t*)(__readgsqword(0x60) + 0x10);;

	ov::create_window();



#ifdef DebugBuildMode

	while (true)
	{
		ov::render();
		if ((GetAsyncKeyState)(VK_END))
		{
			safe_call(Sleep)(500);
			break;
		}
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	p_Device->Release();
	p_Object->Release();
	safe_call(DestroyWindow)(globals.OverlayWnd);

#else
	ov::loop();
#endif // DebugBuildMode




	
}
void Start()
{
	SPOOF_FUNC;



#ifdef DebugBuildMode

	AllocConsole();
	FILE* file1{};
	FILE* file2{};
	freopen_s(&file1, "CONIN$", "r", stdin);
	freopen_s(&file2, "CONOUT$", "w", stdout);
	std::cout << "Lazarus Internal Loaded" << std::endl;

	MainWhile();

	system("cls");
	std::cout << "Dll Unload" << std::endl;
	FreeLibraryAndExitThread(MyModule, 1);
#else
	MainWhile();

#endif





}

class InitModules
{
public:
	NTSTATUS WINAPI CallThreads()
	{
		SPOOF_FUNC;
		//const auto start = pThread->pNtCreateThread->NtCreateThreadEx((LPTHREAD_START_ROUTINE)Start, 0, 0);
		//pThread->pNtCreateThread->NtCloseHandle((HANDLE)start);
		const auto start = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Start, 0, 0, 0);
		CloseHandle(start);
		return TRUE;
	}

}; InitModules pInitModules;

class attach
{
public:
	__declspec() attach* init_thread(HMODULE hModule, void* hBuffer);
}; attach* pAttach;

__declspec() attach* attach::init_thread(HMODULE hModule, void* hBuffer)
{

	SPOOF_FUNC;


#ifdef DebugBuildMode
	MyModule = hModule;
#endif // DebugBuildMode

	
	pInitModules.CallThreads();

	return nullptr;
}

BOOL __stdcall DllMain(HMODULE hModule, DWORD hReasons, LPVOID hBuffer)
{
	SPOOF_FUNC;
	if (hReasons != DLL_PROCESS_ATTACH) return FALSE;

	pAttach->init_thread(hModule, hBuffer);

	return TRUE;
}
