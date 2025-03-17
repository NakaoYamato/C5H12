#include "DepthOfField.h"

#include <imgui.h>

#include "../../Library/Camera/Camera.h"

// XVˆ—
void DepthOfField::Update(float elapsedTime)
{
    // ƒJƒƒ‰î•ñ‚ğæ“¾
    DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&Camera::Instance().GetView());
    DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&Camera::Instance().GetProjection());
    _data.view = Camera::Instance().GetView();
    DirectX::XMStoreFloat4x4(&_data.invViewProjection, DirectX::XMMatrixInverse(nullptr, V * P));
}

void DepthOfField::DrawGui()
{
    ImGui::SliderFloat("focus distance", &_data.focusDistance, 0.1f, 500.0f);
    ImGui::SliderFloat("dof range", &_data.dofRange, 1.0f, 100);
}

std::unordered_map<std::string, float> DepthOfField::GetCurrentData()
{
    std::unordered_map<std::string, float> parameter;
    parameter["focusDistance"] = _data.focusDistance;
    parameter["dofRange"] = _data.dofRange;
    return parameter;
}

void DepthOfField::SetData(std::unordered_map<std::string, float>& parameter)
{
    {
        auto iter = parameter.find("focusDistance");
        if (iter != parameter.end())
            _data.focusDistance = (*iter).second;
    }
    {
        auto iter = parameter.find("dofRange");
        if (iter != parameter.end())
            _data.dofRange = (*iter).second;
    }
}

void DepthOfField::UpdateConstantBuffer(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer)
{
    immediateContext->UpdateSubresource(constantBuffer, 0, 0, &_data, 0, 0);
}
