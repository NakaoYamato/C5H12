#include "DebugSupporter.h"

#include <Windows.h>
// 先にWindowsをインクルードしないと"No Target Architecture"が出て怒られる
#include <debugapi.h>
#include <imgui.h>
#include <ImGuizmo.h>

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Converter/ToString.h"

namespace Debug
{
    std::vector<std::string>    _debugStrings;
    DebugInput                  _debugInput;
    DebugCamera                 _debugCamera;
    std::unique_ptr<DebugRenderer> _debugRenderer;

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
            _debugStrings.push_back(str);
            OutputDebugStringA(str.c_str());
        }
        void String(std::wstring str)
        {
            _debugStrings.push_back(ToString(str));
            OutputDebugStringW(str.c_str());
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
        DirectX::XMFLOAT4X4* transform)
    {
        // 1フレームに複数のギズモを使用できないようにしている
        if (_useGuizmo)return false;
        _useGuizmo = true;
        return ImGuizmo::Manipulate(
            &view._11, &projection._11, _guizmoOperation, _guizmoMode,
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

                static const char* operationNames[] =
                {
                    "TRANSLATE",
                    "ROTATE",
                    "SCALE",
                    "BOUNDS"
                };
                int gO = static_cast<int>(_guizmoOperation);
                if (ImGui::Combo(u8"ギズモ操作", &gO, operationNames, _countof(operationNames)))
                {
                    _guizmoOperation = static_cast<ImGuizmo::OPERATION>(gO);
                }
                static const char* modeNames[] =
                {
                    "LOCAL",
                    "WORLD"
                };
                int gM = static_cast<int>(_guizmoMode);
                if (ImGui::Combo(u8"ギズモ座標系", &gM, modeNames, _countof(modeNames)))
                {
                    _guizmoMode = static_cast<ImGuizmo::MODE>(gM);
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
