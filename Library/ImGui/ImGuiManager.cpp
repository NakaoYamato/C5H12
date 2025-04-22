#include "ImGuiManager.h"

#include "ImGuiProfiler.h"
#include "./ImGuizmo/ImGuizmo.h"

HWND ImGuiManager::hWnd;

void ImGuiManager::Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* dc,
	bool* isPaused, void (*setPause)(bool), int maxThreads)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().Fonts->AddFontFromFileTTF("./Data/Font/ImGui/ArialUni.ttf",
		20.0f, nullptr, glyphRangesJapanese);
	ImGuiIO& id = ImGui::GetIO();
	(void)id;
	id.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	id.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	id.ConfigFlags &= ImGuiConfigFlags_NavEnableGamepad;
	id.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	id.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	id.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;
	id.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
	// --- ウィンドウの深度変更に対応 ---
	id.ConfigViewportsNoDefaultParent = true;
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(device, dc);

	ImGui::StyleColorsClassic();

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_::ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.2f);


	ProfileInitialize(isPaused, setPause, maxThreads);

	ProfileThreadName(0, "Main Thread");
}

void ImGuiManager::Uninitialize()
{
	ProfileShutdown();
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiManager::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ProfileNewFrame();

	DockSpace();

	// ギズモ開始
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImVec2 size = ImGui::GetIO().DisplaySize;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);
}

void ImGuiManager::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

// --- 参考 : https://gist.github.com/moebiussurfing/8dbc7fef5964adcd29428943b78e45d2
void ImGuiManager::DockSpace()
{
	// ---ウィンドウのフラグ設定 ---
	static ImGuiWindowFlags windowFlags =
		ImGuiDockNodeFlags_PassthruCentralNode |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;


	const ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;

	// --- ウィンドウの設定 ---
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->GetWorkPos());				// 位置の設定
	ImGui::SetNextWindowSize(viewport->Size, ImGuiCond_Always);		// サイズの設定
	ImGui::SetNextWindowViewport(viewport->ID);						// IDの設定
	ImGui::SetNextWindowBgAlpha(0.0f);								// 背景の透明度の設定

	// --- スタイルの設定 ---
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	// --- 作成 ---
	ImGui::Begin("DockSpace", NULL, windowFlags);
	ImGui::PopStyleVar(3);

	ImGuiID dockSpaceID = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockSpaceID, ImVec2(0.0f, 0.0f), dockFlags);

	ImGui::End();
}
