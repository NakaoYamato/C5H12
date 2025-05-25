#include "DebugSupporter.h"

#include <Windows.h>
// 先にWindowsをインクルードしないと"No Target Architecture"が出て怒られる
#include <debugapi.h>
#include <imgui.h>
#include <ImGuizmo.h>
#include <mutex>

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Algorithm/Converter.h"

static char s_path_buffer[MAX_PATH];

namespace Debug
{
    std::vector<std::string>    _debugStrings;
    DebugInput                  _debugInput;
    DebugCamera                 _debugCamera;
    std::unique_ptr<DebugRenderer> _debugRenderer;
	std::mutex                 _debugMutex;

    bool _showDemoGui = false;
    bool _showCameraGui = false;
    bool _useGuizmo = false;

    ImGuizmo::OPERATION _guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
    ImGuizmo::MODE _guizmoMode = ImGuizmo::MODE::WORLD;

    namespace Input
    {
        /// 指定のボタンが有効化かどうか
        bool IsActive(DebugButton button)
        {
            return _debugInput.buttonData & button;
        }
    }

    namespace Output
    {
        /// デバッグ文字出力
        void String(std::string str)
        {
			std::lock_guard<std::mutex> lock(_debugMutex);
            _debugStrings.push_back(str);
            OutputDebugStringA(str.c_str());
        }
        void String(std::wstring str)
        {
            std::lock_guard<std::mutex> lock(_debugMutex);
            _debugStrings.push_back(ToString(str));
            OutputDebugStringW(str.c_str());
        }
        void String(int value)
        {
			Output::String(std::to_string(value));
        }
        void String(float value)
        {
            Output::String(std::to_string(value));
        }
        void String(const Vector3& value)
        {
            Debug::Output::String(value.x);
            Debug::Output::String("\t");
            Debug::Output::String(value.y);
            Debug::Output::String("\t");
            Debug::Output::String(value.z);
        }
    }

    namespace Renderer
    {
        /// 箱描画
        void DrawBox(const Vector3& position, const Vector3& angle, const Vector3& size, const Vector4& color)
        {
            _debugRenderer->DrawBox(position, angle, size, color);
        }
        void DrawBox(const DirectX::XMFLOAT4X4& transform, const Vector4& color)
        {
            _debugRenderer->DrawBox(transform, color);
        }
        /// 球描画
        void DrawSphere(const Vector3& position, float radius, const Vector4& color)
        {
            _debugRenderer->DrawSphere(position, radius, color);
        }
        /// カプセル描画
        void DrawCapsule(const DirectX::XMFLOAT4X4& transform, float radius, float height, const Vector4& color)
        {
            _debugRenderer->DrawCapsule(transform, radius, height, color);
        }
        /// カプセル描画
        void DrawCapsule(const Vector3& start, const Vector3& end, float radius, const Vector4& color)
        {
            _debugRenderer->DrawCapsule(start, end, radius, color);
        }
        /// 骨描画
        void DrawBone(const DirectX::XMFLOAT4X4& transform, float length, const Vector4& color)
        {
            _debugRenderer->DrawBone(transform, length, color);
        }
        /// 矢印描画
        void DrawArrow(const Vector3& start, const Vector3& target, float radius, const Vector4& color)
        {
            _debugRenderer->DrawArrow(start, target, radius, color);
        }
        /// 軸描画
        void DrawAxis(const DirectX::XMFLOAT4X4& transform)
        {
            _debugRenderer->DrawAxis(transform);
        }
        /// グリッド描画
        void DrawGrid(int subdivisions, float scale)
        {
            _debugRenderer->DrawGrid(subdivisions, scale);
        }
        void AddVertex(const Vector3& position)
        {
            _debugRenderer->AddVertex(position);
        }
        /// 描画実行
        void Render(
            const DirectX::XMFLOAT4X4& view,
            const DirectX::XMFLOAT4X4& projection)
        {
            _debugRenderer->Render(Graphics::Instance().GetDeviceContext(), view, projection);
        }
    }

    namespace Dialog
    {
        DialogResult OpenFileName(
            std::string& filepath,
            std::string& currentDirectory,
            const char* filter,
            const char* title,
            HWND hwnd, 
            bool multi_select)
        {
            char path[MAX_PATH]{};
            // 初期パス設定
            char dirname[MAX_PATH];
            if (filepath[0] != '0')
            {
                // ディレクトリパス取得
                ::_splitpath_s(path, nullptr, 0, dirname, MAX_PATH, nullptr, 0, nullptr, 0);
            }
            else
            {
                path[0] = dirname[0] = '\0';
            }
            if ((dirname[0] == '\0'))
            {
                strcpy_s(dirname, MAX_PATH, s_path_buffer);
            }
            // lpstrInitialDir は \ でないと受け付けない
            for (char* p = dirname; *p != '\0'; p++)
            {
                if (*p == '/')
                    *p = '\\';
            }

            if (filter == nullptr)
            {
                filter = "All Files\0*.*\0\0";
            }

            // 構造体セット
            OPENFILENAMEA ofn;
            memset(&ofn, 0, sizeof(OPENFILENAMEA));
            ofn.lStructSize = sizeof(OPENFILENAMEA);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = filter;
            ofn.nFilterIndex = 1;
            ofn.lpstrFile = path;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrTitle = title;
            ofn.lpstrInitialDir = (dirname[0] != '\0') ? dirname : nullptr;
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
            if (multi_select)
            {
                ofn.Flags |= OFN_ALLOWMULTISELECT | OFN_EXPLORER;
            }

            // カレントディレクトリ取得
            char currentDir[MAX_PATH];
            if (!::GetCurrentDirectoryA(MAX_PATH, currentDir))
            {
                currentDir[0] = '\0';
            }

            // ダイアログオープン
            if (::GetOpenFileNameA(&ofn) == FALSE)
            {
                return DialogResult::Cancel;
            }

            // カレントディレクトリ復帰
            if (currentDir[0] != '\0')
            {
                ::SetCurrentDirectoryA(currentDir);
            }

            // 最終パスを記憶
            strcpy_s(s_path_buffer, MAX_PATH, path);

            filepath = path;
            currentDirectory = currentDir;

            return DialogResult::OK;
        }

        DialogResult SaveFileName(
            char* filepath, 
            int size, 
            const char* filter, 
            const char* title, 
            const char* ext, 
            HWND hwnd)
        {
            // 初期パス設定
            char dirname[MAX_PATH];
            if (filepath[0] != '0')
            {
                // ディレクトリパス取得
                ::_splitpath_s(filepath, nullptr, 0, dirname, MAX_PATH, nullptr, 0, nullptr, 0);
            }
            else
            {
                filepath[0] = dirname[0] = '\0';
            }
            if ((dirname[0] == '\0'))
            {
                strcpy_s(dirname, MAX_PATH, s_path_buffer);
            }
            // lpstrInitialDir は \ でないと受け付けない
            for (char* p = dirname; *p != '\0'; p++)
            {
                if (*p == '/')
                    *p = '\\';
            }

            if (filter == nullptr)
            {
                filter = "All Files\0*.*\0\0";
            }

            // 構造体セット
            OPENFILENAMEA	ofn;
            memset(&ofn, 0, sizeof(OPENFILENAMEA));
            ofn.lStructSize = sizeof(OPENFILENAMEA);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = filter;
            ofn.nFilterIndex = 1;
            ofn.lpstrFile = filepath;
            ofn.nMaxFile = size;
            ofn.lpstrTitle = title;
            ofn.lpstrInitialDir = (dirname[0] != '\0') ? dirname : nullptr;
            ofn.lpstrDefExt = ext;
            ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;

            // カレントディレクトリ取得
            char current_dir[MAX_PATH];
            if (!::GetCurrentDirectoryA(MAX_PATH, current_dir))
            {
                current_dir[0] = '\0';
            }

            // ダイアログオープン
            if (::GetSaveFileNameA(&ofn) == FALSE)
            {
                return DialogResult::Cancel;
            }

            // カレントディレクトリ復帰
            if (current_dir[0] != '\0')
            {
                ::SetCurrentDirectoryA(current_dir);
            }

            // 最終パスを記憶
            strcpy_s(s_path_buffer, MAX_PATH, filepath);

            return DialogResult::OK;
        }
    }

    void Initialize()
    {
        _debugRenderer = std::make_unique<DebugRenderer>(Graphics::Instance().GetDevice());
    }
    void Update(float elapsedTime)
    {
        _debugCamera.Update(elapsedTime);
        _useGuizmo = false;
    }

    bool Guizmo(
        const DirectX::XMFLOAT4X4& view, 
        const DirectX::XMFLOAT4X4& projection,
        DirectX::XMFLOAT4X4* transform,
        int guizmoOperation,
        int guizmoMode)
    {
        // 1フレームに複数のギズモを使用できないようにしている
        if (_useGuizmo)return false;
        _useGuizmo = true;
        return ImGuizmo::Manipulate(
            &view._11, &projection._11,
            guizmoOperation != -1 ?
                (ImGuizmo::OPERATION)guizmoOperation : _guizmoOperation,
            guizmoMode != -1 ?
                (ImGuizmo::MODE)guizmoMode : _guizmoMode,
            &transform->_11,
            nullptr);
    }

    void DrawGui()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu(u8"デバッグ"))
            {
                ImGui::Checkbox(u8"デバッグカメラ", &_showCameraGui);

                ImGui::Checkbox(u8"IMGUIデモ", &_showDemoGui);

                if (ImGui::BeginMenu(u8"ギズモ"))
                {
                    static const char* operationNames[] =
                    {
                        u8"位置",
                        u8"回転",
                        u8"スケール",
                        "BOUNDS"
                    };
                    int gO = static_cast<int>(_guizmoOperation);
                    if (ImGui::Combo(u8"編集モード", &gO, operationNames, _countof(operationNames)))
                    {
                        _guizmoOperation = static_cast<ImGuizmo::OPERATION>(gO);
                    }
                    static const char* modeNames[] =
                    {
                        u8"ローカル",
                        u8"ワールド"
                    };
                    int gM = static_cast<int>(_guizmoMode);
                    if (ImGui::Combo(u8"座標系", &gM, modeNames, _countof(modeNames)))
                    {
                        _guizmoMode = static_cast<ImGuizmo::MODE>(gM);
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            {
                auto DrawDebugFlagGui = [&](unsigned int key, std::string name)
                    {
                        ImVec4 color = ImVec4(1, 1, 1, 1);
                        if (Debug::Input::IsActive(key))
                        {
                            color = ImVec4(1, 0, 0, 1);
                            name += ":Active";
                        }
                        else
                        {
                            name += ":Deactive";
                        }
                        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, color);
                        if (ImGui::MenuItem(name.c_str()))
                        {
                            Debug::GetDebugInput()->buttonData ^= key;
                        }
                        ImGui::PopStyleColor();
                    };

                DrawDebugFlagGui(DebugInput::BTN_F4, "DebugCamera");
                DrawDebugFlagGui(DebugInput::BTN_F5, "FixedTime");
                DrawDebugFlagGui(DebugInput::BTN_F7, "DebugDraw");
            }

            ImGui::EndMainMenuBar();
        }

        if (_showCameraGui)
        {
            if (ImGui::Begin(u8"デバッグカメラ"))
            {
                _debugCamera.DrawGui();
            }
            ImGui::End();
        }

        if (_showDemoGui)
        {
            ImGui::ShowDemoWindow();
        }
    }
    DebugInput* GetDebugInput()
    {
        return &_debugInput;
    }
    DebugCamera* GetDebugCamera()
    {
        return &_debugCamera;
    }
    DebugRenderer* GetDebugRenderer()
    {
        return _debugRenderer.get();
    }
}
