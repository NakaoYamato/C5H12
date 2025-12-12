#include "Framework.h"

#include "../Graphics/Graphics.h"
#include "../Graphics/GpuResourceManager.h"
#include "../ImGui/ImGuiManager.h"
#include "../Input/Input.h"
#include "../Audio/AudioSystem.h"
#include "../DebugSupporter/DebugSupporter.h"
#include "../Model/ModelResourceManager.h"
#include "../PostProcess/PostProcessManager.h"
#include "../JobSystem/JobSystem.h"
#include "../Scene/SceneManager.h"
#include "../Resource/ResourceManager.h"

#include <Dbt.h>

// 垂直同期間隔設定
int Framework::syncInterval = 0;
// ドロップされたファイルパス
std::wstring Framework::filePath;
// プロファイラーの描画フラグ
static bool imguiProfilerIsPause = false;

/// ゲームループの実行
int Framework::Run()
{
    // ウィンドウからのメッセージを受け取る変数
    MSG msg{};

    // 初期化処理　失敗したら終了
    if (!Initialize())
        return 0;

    // ゲームループ
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // 前フレームからの経過時間計算
            _tictoc.Tick();
            CalcFrameStatus();

            // 固定間隔更新処理
            fixedUpdateTimer += _tictoc.TimeInterval();
            if (fixedUpdateTimer - _FIXED_UPDATE_RATE >= 0.0f)
            {
                FixedUpdate();
                fixedUpdateTimer = fixedUpdateTimer - _FIXED_UPDATE_RATE;
            }

            // 更新処理
            Update(_tictoc.TimeInterval());

            Render();
        }
    }

    return Uninitialize() ? static_cast<int>(msg.wParam) : 0;
}

/// ウィンドウのメッセージを受け取るためのコールバック関数
LRESULT Framework::HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
#ifdef USE_IMGUI
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
#endif

    int num{}, i{};
    HDROP hdrop{};
    TCHAR filename[100]{};

    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps{};
        BeginPaint(hwnd, &ps);

        EndPaint(hwnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CREATE:
        break;
    case WM_KEYDOWN:
        // 仮想キーボード入力取得
        // 参考:https://learn.microsoft.com/ja-jp/windows/win32/inputdev/virtual-key-codes
#ifdef _DEBUG
        switch (wparam)
        {
        //case VK_ESCAPE:
        //    PostMessage(hwnd, WM_CLOSE, 0, 0);
        //    break;
            // デバッグ中のみ有効化
        case VK_F1:
        case VK_F2:
        case VK_F3:
        case VK_F4:
        case VK_F5:
        case VK_F6:
        case VK_F7:
        case VK_F8:
        case VK_F9:
        case VK_F10:
        case VK_F11:
        case VK_F12:
            // 指定のキーを押すとそのビットを反転
            Debug::GetDebugInput()->buttonData ^= (1 << (wparam - VK_F1));
            break;
        }
#endif
        break;
    case WM_ENTERSIZEMOVE:
        _tictoc.Stop();
        break;
    case WM_EXITSIZEMOVE:
        _tictoc.Start();
        break;
    case WM_MOUSEWHEEL:
        if (Input::Instance().GetMouseInput() != nullptr)
            Input::Instance().GetMouseInput()->SetWheel(GET_WHEEL_DELTA_WPARAM(wparam));
        break;
    case WM_DROPFILES:
        // ファイルがドロップされた時の処理
        hdrop = (HDROP)wparam;
        //ドロップされたFile数
        num = DragQueryFile(hdrop, -1, NULL, 0);

        // 本来はドロップされた数だけfilanameを取得できるが今回は最後のfilenameのみ取得する
        for (i = 0; i < num; i++)
        {
            DragQueryFile(hdrop, i, filename, sizeof(filename) / sizeof(TCHAR));
            filePath = filename;
        }
        DragFinish(hdrop);

        break;
    case WM_SIZE:
    {

#ifdef X3DGP_FULLSCREEN
        // 画面サイズが変更されたとき
        //RECT client_rect{};
        //GetClientRect(hwnd, &client_rect);
        //UINT width = client_rect.right - client_rect.left;
        //UINT height = client_rect.bottom - client_rect.top;
        //if (width != 0 && height != 0)
        //    Graphics::Instance().OnSizeChanged(width, height);
#endif
        break;
    }
    case WM_DEVICECHANGE:
		// USBデバイスの接続・切断時の処理
        if (Input::Instance().GetDirectInput() != nullptr)
        {
            // DirectInputのゲームパッド検索
            Input::Instance().GetDirectInput()->SearchGamepad();
        }
        break;
    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    return 0;
}

bool Framework::Initialize() const
{
    // Graphicsの初期化
    Graphics::Instance().Initialize(_hwnd, _hInstance, FULLSCREEN);

    // ポストプロセス管理者初期化
    PostProcessManager::Instance().Initialize(Graphics::Instance().GetDevice(),
        static_cast<uint32_t>(Graphics::Instance().GetScreenWidth()),
        static_cast<uint32_t>(Graphics::Instance().GetScreenHeight()));

    // 入力監視クラスの初期化
    Input::Instance().Initialize(_hwnd, _hInstance);

    // リソースの初期化
    ResourceManager::Instance().Initialize();

    // シーンの初期化
    SceneManager::Instance().Initialize();

    // デバッグの初期化
    Debug::Initialize();

	// ジョブシステム初期化
    // サーバーのスレッドで最大２個必要なのでその分減らしている
    JobSystem::Instance().Initialize(10);
    //JobSystem::Instance().Initialize(std::max<size_t>(6, 1));

    // ImGui初期化
    ImGuiManager::Initialize(_hwnd,
        Graphics::Instance().GetDevice(),
        Graphics::Instance().GetDeviceContext(),
        &imguiProfilerIsPause,
        [](bool pause) {imguiProfilerIsPause = pause; },
        static_cast<int>(JobSystem::Instance().GetNumThreads()) + 1);

    return true;
}

void Framework::Update(float elapsedTime)
{
#ifdef USE_IMGUI
    // IMGUI更新処理
    ImGuiManager::Update();
#endif

    // F5キー有効化で時間停止
    if (Debug::Input::IsActive(DebugInput::BTN_F5))
        elapsedTime = 0.0f;
    // デバッグ用ゲーム速度適応
    elapsedTime = elapsedTime * Debug::GetGameSpeed();

    // 入力監視クラスの更新
    {
        ProfileScopedSection_3(0, "Input::Update", ImGuiControl::Profiler::Dark);
        Input::Instance().Update();
    }

    // デバッグの更新
    {
        ProfileScopedSection_3(0, "Debug::Update", ImGuiControl::Profiler::Dark);
        Debug::Update(elapsedTime);
    }

    // シーンの更新
    {
        ProfileScopedSection_3(0, "Scene::Update", ImGuiControl::Profiler::Dark);
        SceneManager::Instance().Update(elapsedTime);
    }

	// シャドウマップの更新
    {
		ProfileScopedSection_3(0, "ShadowMap::Update", ImGuiControl::Profiler::Dark);
        Graphics::Instance().GetCascadedShadowMap()->Update(elapsedTime);
    }

    // ポストエフェクトの更新
    {
		ProfileScopedSection_3(0, "PostProcessManager::Update", ImGuiControl::Profiler::Dark);
        PostProcessManager::Instance().Update(elapsedTime);
    }

    // オーディオ更新
    {
		ProfileScopedSection_3(0, "AudioSystem::Update", ImGuiControl::Profiler::Dark);
        AudioSystem::Instance().Update();
    }
}

/// 一定間隔の更新処理
void Framework::FixedUpdate()
{
    ProfileScopedSection_3(0, "Scene::FixedUpdate", ImGuiControl::Profiler::Dark);
    // シーンの更新
    SceneManager::Instance().FixedUpdate();
}

/// 描画処理
void Framework::Render()
{
    // シーンの描画
    {
        // 別スレッド中にデバイスコンテキストが使われていた場合に
        // 同時アクセスしないように排他制御する
        std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());
		ProfileScopedSection_3(0, "Scene::Render", ImGuiControl::Profiler::Dark);
		SceneManager::Instance().Render();
    }

#ifdef USE_IMGUI
    // F6キーでGUIを非表示
    if (!Debug::Input::IsActive(DebugInput::BTN_F6))
    {
        ProfileScopedSection_3(0, "DrawGui", ImGuiControl::Profiler::Dark);

        // GUIのメニューバーでシーン変更
        SceneManager::Instance().SceneMenuGui();

		// リソース管理クラスのGui描画
		GpuResourceManager::DrawGui(Graphics::Instance().GetDevice());

        // モデルリソース管理クラスのGui描画
        ModelResourceManager::Instance().DrawGui();

        // シーンのGui描画
        SceneManager::Instance().DrawGui();

        // ポストプロセスのGui描画
        PostProcessManager::Instance().DrawGui();

        // 入力管理クラスのGUI描画
        Input::Instance().DrawGui();

        // 描画管理者のGUI描画
        Graphics::Instance().DrawGui();

        // ジョブシステムのGUI描画
        JobSystem::Instance().DrawGui();

		// リソース管理クラスのGui描画
		ResourceManager::Instance().DrawGui();

        // デバッグのGui描画
        Debug::DrawGui();
    }

    // ImGui描画
    {
        // 別スレッド中にデバイスコンテキストが使われていた場合に
        // 同時アクセスしないように排他制御する
        std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());
        ImGuiManager::Render();
    }
#endif

    // バックバッファに描画した画を画面に表示する。
    {
		ProfileScopedSection_3(0, "Graphics::Present", ImGuiControl::Profiler::Dark);
        Graphics::Instance().Present(syncInterval);
    }
}

bool Framework::Uninitialize()
{
    // ImGui終了化
    ImGuiManager::Uninitialize();

    // シーン終了化
    SceneManager::Instance().Finalize();

    // オーディオ終了化
    AudioSystem::Instance().Finalize();

    // ジョブシステム終了
    JobSystem::Instance().Finalize();

    // フルスクリーン時の終了処理
    BOOL fullscreen = 0;
    Graphics::Instance().GetSwapChain()->GetFullscreenState(&fullscreen, 0);
    if (fullscreen)
    {
        Graphics::Instance().GetSwapChain()->SetFullscreenState(FALSE, 0);
    }

    return true;
}

/// フレームレートの計算
void Framework::CalcFrameStatus()
{
    ++_elapsedFrame;
    if ((_tictoc.TimeStamp() - _elapsedTime) >= 1.0f)
    {
        _fps = static_cast<float>(_elapsedFrame);
        std::ostringstream outs;
        outs.precision(6);
#if _DEBUG
        // タイトルバーにFPS表示
        outs << _fps << "/" << "FrameTime:" << 1000.0f / _fps << "(ms)";
        SetWindowTextA(_hwnd, outs.str().c_str());
#else
        //SetWindowTextW(hwnd_, L"タイトル");
        outs << _fps << "/" << "FrameTime:" << 1000.0f / _fps << "(ms)";
        SetWindowTextA(_hwnd, outs.str().c_str());
#endif

        _elapsedFrame = 0;
        _elapsedTime += 1.0f;
    }
}
