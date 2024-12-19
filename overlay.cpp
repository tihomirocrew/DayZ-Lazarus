#include "safecall.h"
#include "lazy_importer.hpp"
#include "xorstr.hpp"
#include "call_stack_spoofer.hpp"
#include "overlay.h"
#include <iostream>
#include <thread>

//#include "xorstr.h"

//#include "includes/modules/security/xorstr.h"
//#include "includes/modules/modules.hpp"
#pragma warning(disable : 4996)
IDirect3D9Ex* p_Object = NULL;
IDirect3DDevice9Ex* p_Device = NULL;
D3DPRESENT_PARAMETERS p_Params = { NULL };
WNDPROC oriWndProc = NULL;

bool ov::toggle_menu = 1;

void ov::create_window(bool console)
{
	SPOOF_FUNC;

	globals.Width = safe_call(GetSystemMetrics)(SM_CXSCREEN);
	globals.Height =safe_call(GetSystemMetrics)(SM_CYSCREEN);

	while (!globals.OverlayWnd)
	{
		globals.OverlayWnd =safe_call(FindWindowW)(__(L"CEF-OSC-WIDGET"), __(L"NVIDIA GeForce Overlay"));
		safe_call(Sleep)(50);
	}
	//std::cout << "OverlayHwnd:" << globals.OverlayWnd << std::endl;
	safe_call(ShowWindow)(globals.OverlayWnd, SW_SHOW);

	safe_call(SetWindowLongA)(globals.OverlayWnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);

	safe_call(SetWindowLongA)(
		globals.OverlayWnd,
		-20,
		static_cast<LONG_PTR>(
			static_cast<int>(safe_call(GetWindowLongA)(globals.OverlayWnd, -20)) | 0x20
			)
		);

	MARGINS margin = { -1, -1, -1, -1 };
	safe_call(DwmExtendFrameIntoClientArea)(globals.OverlayWnd, &margin);

	safe_call(SetLayeredWindowAttributes)(globals.OverlayWnd, 0x000000, 0xFF, 0x02);

	safe_call(SetWindowPos)(
		globals.OverlayWnd,
		HWND_TOPMOST,
		0, 0, 0, 0,
		0x0002 | 0x0001
		);

	safe_call(UpdateWindow)(globals.OverlayWnd);


	setup_directx(globals.OverlayWnd);
}

void ov::setup_directx(HWND hWnd)
{
    if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
        return;
    ZeroMemory(&p_Params, sizeof(p_Params));
    p_Params.BackBufferWidth = globals.Width;
    p_Params.BackBufferHeight = globals.Height;
    p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
    p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
    p_Params.AutoDepthStencilFormat = D3DFMT_D16;
    p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    p_Params.EnableAutoDepthStencil = TRUE;
    p_Params.hDeviceWindow = hWnd;
    p_Params.FullScreen_RefreshRateInHz = 0;
    p_Params.Windowed = TRUE;

    if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, globals.OverlayWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
    {
        p_Object->Release();
        //exit(4);
        return;
    }

    //IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX9_Init(p_Device);

    // original font, but looks ugly with cyrillic letters
    //io.Fonts->AddFontFromFileTTF(__("simhei.ttf"), 14, NULL, io.Fonts->GetGlyphRangesChineseFull());
	//globals.CompassFont = io.Fonts->AddFontFromFileTTF(__("simhei.ttf"), 20, NULL, io.Fonts->GetGlyphRangesChineseFull());
    
    // that's better
    io.Fonts->AddFontFromFileTTF(__("C:\\Windows\\Fonts\\Verdana.ttf"), 14, NULL, io.Fonts->GetGlyphRangesCyrillic());
	globals.CompassFont = io.Fonts->AddFontFromFileTTF(__("C:\\Windows\\Fonts\\Verdana.ttf"), 20, NULL, io.Fonts->GetGlyphRangesCyrillic());

    p_Object->Release();
}

void ov::render()
{
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
   
  
	io.DisplaySize = ImVec2(globals.Width, globals.Height);
    io.DeltaTime = 0.01f;
    ImGui::NewFrame();

  
    if (ov::toggle_menu)
    {
        draw_abigsquare();
    }

    if (safe_call(GetAsyncKeyState)(VK_F1))
    {
        ov::toggle_menu = !ov::toggle_menu;
        safe_call(Sleep)(300);
    }

    main_cheat_handler();



    ImGui::EndFrame();
    p_Device->SetRenderState(D3DRS_ZENABLE, false);
    p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
    p_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
    p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

    if (p_Device->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        p_Device->EndScene();
    }
    HRESULT result = p_Device->Present(NULL, NULL, NULL, NULL);

    if (result == D3DERR_DEVICELOST && p_Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
    {
        ImGui_ImplDX9_InvalidateDeviceObjects();
        p_Device->Reset(&p_Params);
        ImGui_ImplDX9_CreateDeviceObjects();
    }
}

void ov::loop()
{


    while (true)
    {
		ov::render();
    }


}
