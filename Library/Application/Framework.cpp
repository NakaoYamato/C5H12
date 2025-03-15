#include "Framework.h"

#include "../Graphics/Graphics.h"
#include "../ImGui/ImGuiManager.h"
#include "../Input/Input.h"
#include "../DebugSupporter/DebugSupporter.h"
#include "../ResourceManager/ModelResourceManager.h"
#include "../PostProcess/PostProcessManager.h"
//#include "../Effekseer/EffectManager.h"
#include "../Renderer/MeshRenderer.h"
#include "../Renderer/PrimitiveRenderer.h"
#include "../Renderer/ShapeRenderer.h"

#include "../Scene/SceneManager.h"

// 垂直同期間隔設定
int Framework::syncInterval = 0;
std::wstring Framework::filePath;

int Framework::Run()
{
    // ウィンドウからのメッセージを受け取る変数
    MSG msg{};

    if (!Initialize())
        return 0;

    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            tictoc_.Tick();
            CalcFrameStatus();
            Update(tictoc_.TimeInterval());
            if (elapsed1Second_)
            {
                FixedUpdate();
                elapsed1Second_ = false;
            }
            Render(tictoc_.TimeInterval());
        }
    }

    return Uninitialize() ? static_cast<int>(msg.wParam) : 0;
}

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
        switch (wparam)
        {
        case VK_ESCAPE:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
#ifdef _DEBUG
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
            Debug::GetDebugInput()->buttonData_ ^= (1 << (wparam - VK_F1));
            break;
#endif
        }
        break;
    case WM_ENTERSIZEMOVE:
        tictoc_.Stop();
        break;
    case WM_EXITSIZEMOVE:
        tictoc_.Start();
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
        for (i = 0; i < num; i++) {
            DragQueryFile(hdrop, i, filename, sizeof(filename) / sizeof(TCHAR));
            filePath = filename;
        }
        DragFinish(hdrop);

        break;
    case WM_SIZE:
    {

#ifdef X3DGP_FULLSCREEN
        // 画面サイズが変更されたとき
        RECT client_rect{};
        GetClientRect(hwnd, &client_rect);
        UINT width = client_rect.right - client_rect.left;
        UINT height = client_rect.bottom - client_rect.top;
        if (width != 0 && height != 0)
            Graphics::Instance().OnSizeChanged(width, height);
#endif
        break;
    }
    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    return 0;
}

bool Framework::Initialize() const
{
    // Graphicsの初期化
    Graphics::Instance().Initialize(hwnd_, FULLSCREEN);

    // 各種レンダラー作成
    MeshRenderer::Initialize(Graphics::Instance().GetDevice());
    PrimitiveRenderer::Initialize(Graphics::Instance().GetDevice());
    ShapeRenderer::Initialize(Graphics::Instance().GetDevice());

    // ポストプロセス管理者初期化
    PostProcessManager::Instance().Initialize(Graphics::Instance().GetDevice(),
        static_cast<uint32_t>(Graphics::Instance().GetScreenWidth()),
        static_cast<uint32_t>(Graphics::Instance().GetScreenHeight()));

    // 入力監視クラスの初期化
    Input::Instance().Initialize(hwnd_);

    //// エフェクトマネージャー初期化
    //EffectManager::Instance().Initialize();

    // シーンの初期設定
    SceneManager::Instance().ChangeScene(u8"デバッグ");

    // デバッグの初期化
    Debug::Initialize();

    // ImGui初期化
    ImGuiManager::Initialize(hwnd_,
        Graphics::Instance().GetDevice(),
        Graphics::Instance().GetDeviceContext());

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

    // 入力監視クラスの更新
    Input::Instance().Update();

    // デバッグの更新
    Debug::Update(elapsedTime);

    // シーンの更新
    SceneManager::Instance().Update(elapsedTime);

    //// 3Dエフェクト更新
    //EffectManager::Instance().Update(elapsedTime);
}

void Framework::FixedUpdate()
{
    // シーンの更新
    SceneManager::Instance().FixedUpdate();
}

void Framework::Render(float elapsedTime)
{
    // 別スレッド中にデバイスコンテキストが使われていた場合に
    // 同時アクセスしないように排他制御する
    std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());

    // シーンの描画
    SceneManager::Instance().Render();

#ifdef USE_IMGUI
    // F6キーでGUIを非表示
    if (!Debug::Input::IsActive(DebugInput::BTN_F6))
    {
        // GUIのメニューバーでシーン変更
        SceneManager::Instance().SceneMenuGui();

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

        // デバッグのGui描画
        Debug::DrawGui();
    }

    // ImGui描画
    ImGuiManager::Render();
#endif

    // バックバッファに描画した画を画面に表示する。
    Graphics::Instance().Present(syncInterval);
}

bool Framework::Uninitialize()
{
    // ImGui終了化
    ImGuiManager::Uninitialize();

    // エフェクトマネージャーの終了の後にエフェクトの終了処理を行うとエラーになるので防ぐ
    SceneManager::Instance().Clear();

    // エフェクトマネージャー終了化
    //EffectManager::Instance().Finalize();

    // フルスクリーン時の終了処理
    BOOL fullscreen = 0;
    Graphics::Instance().GetSwapChain()->GetFullscreenState(&fullscreen, 0);
    if (fullscreen)
    {
        Graphics::Instance().GetSwapChain()->SetFullscreenState(FALSE, 0);
    }

    return true;
}

void Framework::CalcFrameStatus()
{
    ++elapsedFrame_;
    if ((tictoc_.TimeStamp() - elapsedTime_) >= 1.0f)
    {
        fps_ = static_cast<float>(elapsedFrame_);
        std::ostringstream outs;
        outs.precision(6);
#if _DEBUG
        outs << fps_ << "/" << "FrameTime:" << 1000.0f / fps_ << "(ms)";
        SetWindowTextA(hwnd_, outs.str().c_str());
#else
        //SetWindowTextW(hwnd_, L"タイトル");
        outs << fps_ << "/" << "FrameTime:" << 1000.0f / fps_ << "(ms)";
        SetWindowTextA(hwnd_, outs.str().c_str());
#endif

        // 1秒が過ぎたフラグをオン
        elapsed1Second_ = true;

        elapsedFrame_ = 0;
        elapsedTime_ += 1.0f;
    }
}
