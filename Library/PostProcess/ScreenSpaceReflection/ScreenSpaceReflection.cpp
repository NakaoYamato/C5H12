#include "ScreenSpaceReflection.h"

#include <imgui.h>

#include "../../Library/Camera/Camera.h"
// 更新処理
void ScreenSpaceReflection::Update(float elapsedTime)
{
    // カメラ情報を取得
    _data.view = Camera::Instance().GetView();
    _data.projection = Camera::Instance().GetProjection();
    DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&Camera::Instance().GetView());
    DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&Camera::Instance().GetProjection());
    DirectX::XMStoreFloat4x4(&_data.invProjection,
        DirectX::XMMatrixInverse(nullptr, P));
    DirectX::XMStoreFloat4x4(&_data.invViewProjection, DirectX::XMMatrixInverse(nullptr, V * P));
}

// デバッグGui描画
void ScreenSpaceReflection::DrawGui()
{
    ImGui::DragFloat(u8"refrectionIntensity", &_data.refrectionIntensity, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat(u8"maxDistance", &_data.maxDistance, 0.1f);
    ImGui::DragFloat(u8"resolution", &_data.resolution, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat(u8"thickness", &_data.thickness, 0.01f, 0.0f, 1.0f);
}

// 現在のデータの取得
std::unordered_map<std::string, float> ScreenSpaceReflection::GetCurrentData()
{
    return std::unordered_map<std::string, float>();
}

// データのセット
void ScreenSpaceReflection::SetData(std::unordered_map<std::string, float>& parameter)
{
}

// 定数バッファの更新
void ScreenSpaceReflection::UpdateConstantBuffer(
    ID3D11DeviceContext* immediateContext,
    ID3D11Buffer* constantBuffer)
{
    immediateContext->UpdateSubresource(constantBuffer, 0, 0, &_data, 0, 0);
}
