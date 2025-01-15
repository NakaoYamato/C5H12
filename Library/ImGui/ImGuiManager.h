#pragma once

#include <windows.h>

// åxçêÇÃó}êß
#pragma warning(push)
#pragma warning(disable:33010)
#pragma warning(disable:33011)
#pragma warning(disable:6011)
#pragma warning(disable:28182)
#pragma warning(disable:26495)
#include "./imgui-docking/imgui.h"
#include "./imgui-docking/imgui_impl_win32.h"
#include "./imgui-docking/imgui_impl_dx11.h"
#include "./imgui-docking/imstb_truetype.h"
#include "./imgui-docking/imgui_internal.h"
#pragma warning(pop)

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];

//---------------------------------------------------
// ImGuiä«óùÉNÉâÉX
//---------------------------------------------------
class ImGuiManager
{
private:
	ImGuiManager() {}
	~ImGuiManager() {}

public:
	static void Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* dc);

	static void Uninitialize();

	static void Update();

	static void Render();

private:
	static HWND hWnd;

	static void DockSpace();
};
