#include "DebugSupporter.h"

#include <Windows.h>
// 先にWindowsをインクルードしないと"No Target Architecture"が出て怒られる
#include <debugapi.h>
#include <imgui.h>

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Converter/ToString.h"

namespace Debug
{
    std::vector<std::string>    debugStrings_;
    DebugInput                  debugInput_;
    DebugCamera                 debugCamera_;
    std::unique_ptr<DebugRenderer> debugRenderer_;

    bool showCameraGui_ = false;

    namespace Input
    {
        /// 指定のボタンが有効化かどうか
        bool IsActive(DebugButton button)
        {
            return debugInput_.buttonData & button;
        }
    }

    namespace Output
    {
        /// デバッグ文字出力
        void String(std::string str)
        {
            debugStrings_.push_back(str);
            OutputDebugStringA(str.c_str());
        }
        void String(std::wstring str)
        {
            debugStrings_.push_back(ToString(str));
            OutputDebugStringW(str.c_str());
        }
    }

    namespace Renderer
    {
        /// 箱描画
        void DrawBox(const Vector3& position, const Vector3& angle, const Vector3& size, const Vector4& color)
        {
            debugRenderer_->DrawBox(position, angle, size, color);
        }
        void DrawBox(const DirectX::XMFLOAT4X4& transform, const Vector4& color)
        {
            debugRenderer_->DrawBox(transform, color);
        }
        /// 球描画
        void DrawSphere(const Vector3& position, float radius, const Vector4& color)
        {
            debugRenderer_->DrawSphere(position, radius, color);
        }
        /// カプセル描画
        void DrawCapsule(const DirectX::XMFLOAT4X4& transform, float radius, float height, const Vector4& color)
        {
            debugRenderer_->DrawCapsule(transform, radius, height, color);
        }
        /// カプセル描画
        void DrawCapsule(const Vector3& start, const Vector3& end, float radius, const Vector4& color)
        {
            debugRenderer_->DrawCapsule(start, end, radius, color);
        }
        /// 骨描画
        void DrawBone(const DirectX::XMFLOAT4X4& transform, float length, const Vector4& color)
        {
            debugRenderer_->DrawBone(transform, length, color);
        }
        /// 矢印描画
        void DrawArrow(const Vector3& start, const Vector3& target, float radius, const Vector4& color)
        {
            debugRenderer_->DrawArrow(start, target, radius, color);
        }
        /// 軸描画
        void DrawAxis(const DirectX::XMFLOAT4X4& transform)
        {
            debugRenderer_->DrawAxis(transform);
        }
        /// グリッド描画
        void DrawGrid(int subdivisions, float scale)
        {
            debugRenderer_->DrawGrid(subdivisions, scale);
        }
        void AddVertex(const Vector3& position)
        {
            debugRenderer_->AddVertex(position);
        }
        /// 描画実行
        void Render(
            const DirectX::XMFLOAT4X4& view,
            const DirectX::XMFLOAT4X4& projection)
        {
            debugRenderer_->Render(Graphics::Instance().GetDeviceContext(), view, projection);
        }
    }
    void Initialize()
    {
        debugRenderer_ = std::make_unique<DebugRenderer>(Graphics::Instance().GetDevice());
    }
    void Update(float elapsedTime)
    {
        debugCamera_.Update(elapsedTime);
    }
    void DrawGui()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu(u8"デバッグ"))
            {
                ImGui::Checkbox(u8"デバッグカメラ", &showCameraGui_);

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

        if (showCameraGui_)
        {
            if (ImGui::Begin(u8"デバッグカメラ"))
            {
                debugCamera_.DrawGui();
            }
            ImGui::End();
        }
    }
    DebugInput* GetDebugInput()
    {
        return &debugInput_;
    }
    DebugCamera* GetDebugCamera()
    {
        return &debugCamera_;
    }
    DebugRenderer* GetDebugRenderer()
    {
        return debugRenderer_.get();
    }
}
