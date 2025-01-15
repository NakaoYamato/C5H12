#include "DebugSupporter.h"

#include <Windows.h>
// 先にWindowsをインクルードしないと"No Target Architecture"が出て怒られる
#include <debugapi.h>
#include <imgui.h>

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Converter/ToString.h"

namespace Debug
{
    /// 指定のボタンが有効化かどうか
    bool IsActive(DebugButton button)
    {
        return DebugSupporter::Instance().GetDebugInput()->buttonData_ & button;
    }
    /// デバッグ文字出力
    void OutputString(std::string str)
    {
        DebugSupporter::Instance().OutputString(str);
    }
    void OutputString(std::wstring str)
    {
        DebugSupporter::Instance().OutputString(str);
    }
    /// 箱描画
    void DrawBox(const Vector3& position, const Vector3& angle, const Vector3& size, const Vector4& color)
    {
        DebugSupporter::Instance().GetDebugRenderer()->DrawBox(position, angle, size, color);
    }
    void DrawBox(const DirectX::XMFLOAT4X4& transform, const Vector4& color)
    {
        DebugSupporter::Instance().GetDebugRenderer()->DrawBox(transform, color);
    }
    /// 球描画
    void DrawSphere(const Vector3& position, float radius, const Vector4& color)
    {
        DebugSupporter::Instance().GetDebugRenderer()->DrawSphere(position, radius, color);
    }
    /// カプセル描画
    void DrawCapsule(const DirectX::XMFLOAT4X4& transform, float radius, float height, const Vector4& color)
    {
        DebugSupporter::Instance().GetDebugRenderer()->DrawCapsule(transform, radius, height, color);
    }
    /// カプセル描画
    void DrawCapsule(const Vector3& start, const Vector3& end, float radius, const Vector4& color)
    {
        DebugSupporter::Instance().GetDebugRenderer()->DrawCapsule(start, end, radius, color);
    }
    /// 骨描画
    void DrawBone(const DirectX::XMFLOAT4X4& transform, float length, const Vector4& color)
    {
        DebugSupporter::Instance().GetDebugRenderer()->DrawBone(transform, length, color);
    }
    /// 矢印描画
    void DrawArrow(const Vector3& start, const Vector3& target, float radius, const Vector4& color)
    {
        DebugSupporter::Instance().GetDebugRenderer()->DrawArrow(start, target, radius, color);
    }
    /// 軸描画
    void DrawAxis(const DirectX::XMFLOAT4X4& transform)
    {
        DebugSupporter::Instance().GetDebugRenderer()->DrawAxis(transform);
    }
    /// グリッド描画
    void DrawGrid(int subdivisions, float scale)
    {
        DebugSupporter::Instance().GetDebugRenderer()->DrawGrid(subdivisions, scale);
    }
    void AddVertex(const Vector3& position)
    {
        DebugSupporter::Instance().GetDebugRenderer()->AddVertex(position);
    }
    /// 描画実行
    void Render(
        const DirectX::XMFLOAT4X4& view,
        const DirectX::XMFLOAT4X4& projection)
    {
        DebugSupporter::Instance().GetDebugRenderer()->Render(Graphics::Instance().GetDeviceContext(), view, projection);
    }
    DebugCamera* GetDebugCamera()
    {
        return DebugSupporter::Instance().GetDebugCamera();
    }
}

/// 初期化
void DebugSupporter::Initialize()
{
    debugRenderer_ = std::make_unique<DebugRenderer>(Graphics::Instance().GetDevice());
}

/// 更新処理
void DebugSupporter::Update(float elapsedTime)
{
    debugCamera_.Update(elapsedTime);
}

/// デバッグGUI表示
void DebugSupporter::DrawGui()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(u8"デバッグ"))
        {
            ImGui::Checkbox(u8"デバッグカメラ", &showCameraGui_);

            ImGui::EndMenu();
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

/// stringを出力に表示
void DebugSupporter::OutputString(std::string str)
{
    debugStrings_.push_back(str);
    OutputDebugStringA(str.c_str());
}

/// stringを出力に表示
void DebugSupporter::OutputString(std::wstring str)
{
    debugStrings_.push_back(ToString(str));
    OutputDebugStringW(str.c_str());
}
