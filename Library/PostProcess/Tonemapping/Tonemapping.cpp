#include "Tonemapping.h"
#include <imgui.h>

void Tonemapping::DrawGui()
{
	if (ImGui::Begin(u8"トーンマッピング"))
	{
		ImGui::SliderFloat("exposure", &data.exposure, 0.0f, 10.0f);
		ImGui::SliderInt("toneType", &data.toneType, 0, 2);
		if (ImGui::TreeNode("Resource"))
		{
			static float textureSize = 512.0f;
			ImGui::DragFloat("TextureSize", &textureSize);
			ImGui::Image(GetColorSRV().Get(),
				{ textureSize ,textureSize });

			ImGui::TreePop();
		}
	}
	ImGui::End();
}

void Tonemapping::UpdateConstantBuffer(ID3D11DeviceContext* immediateContext,
    ID3D11Buffer* constantBuffer)
{
    immediateContext->UpdateSubresource(constantBuffer, 0, 0, &data, 0, 0);
}
