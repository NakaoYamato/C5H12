#include "SkyMap.h"
#include "../Graphics/GpuResourceManager.h"

#include "../HRTrace.h"

#include <imgui.h>
SkyMap::SkyMap(ID3D11Device* device, const wchar_t* filename, const wchar_t* diffuseIEM, const wchar_t* specularIDM)
{
	D3D11_TEXTURE2D_DESC texture2d_desc;
	GpuResourceManager::LoadTextureFromFile(device, filename, _shaderResourceView.GetAddressOf(), &texture2d_desc);

	if (texture2d_desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
	{
		_isTextureCube = true;
	}

	GpuResourceManager::CreateVsFromCso(device, "./Data/Shader/HLSL/SkyMap/SkyMapVS.cso", _vertexShader.GetAddressOf(), NULL, NULL, 0);
	GpuResourceManager::CreatePsFromCso(device, "./Data/Shader/HLSL/SkyMap/SkyMapPS.cso", _pixelShader[0].GetAddressOf());
	GpuResourceManager::CreatePsFromCso(device, "./Data/Shader/HLSL/SkyMap/SkyBoxPS.cso", _pixelShader[1].GetAddressOf());

	if(diffuseIEM)
		GpuResourceManager::LoadTextureFromFile(device, diffuseIEM, _diffuseIEMSRV.GetAddressOf(), &texture2d_desc);
	if(specularIDM)
		GpuResourceManager::LoadTextureFromFile(device, diffuseIEM, _specularIEMSRV.GetAddressOf(), &texture2d_desc);

	GpuResourceManager::LoadTextureFromFile(device, L"./Data/SkyMap/lut_ggx.DDS", _lutSRVs[0].GetAddressOf(), &texture2d_desc);
	GpuResourceManager::LoadTextureFromFile(device, L"./Data/SkyMap/lut_charlie.DDS", _lutSRVs[1].GetAddressOf(), &texture2d_desc);
	GpuResourceManager::LoadTextureFromFile(device, L"./Data/SkyMap/lut_sheen_E.DDS", _lutSRVs[2].GetAddressOf(), &texture2d_desc);

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(Constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&buffer_desc, nullptr, _constantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

void SkyMap::Blit(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	static constexpr int CBIndex = 0;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cacheBuffer;
	dc->PSGetConstantBuffers(CBIndex, 1, cacheBuffer.ReleaseAndGetAddressOf());

	dc->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dc->IASetInputLayout(NULL);

	dc->VSSetShader(_vertexShader.Get(), 0, 0);
	dc->PSSetShader(_isTextureCube ? _pixelShader[1].Get() : _pixelShader[0].Get(), 0, 0);

	dc->PSSetShaderResources(0, 1, _shaderResourceView.GetAddressOf());

	DirectX::XMMATRIX        VP = DirectX::XMLoadFloat4x4(&rc.camera->GetView()) *
		DirectX::XMLoadFloat4x4(&rc.camera->GetProjection());
	Constants data;
	DirectX::XMStoreFloat4x4(&data.inverseViewProjection, DirectX::XMMatrixInverse(NULL, VP));

	dc->UpdateSubresource(_constantBuffer.Get(), 0, 0, &data, 0, 0);
	dc->PSSetConstantBuffers(CBIndex, 1, _constantBuffer.GetAddressOf());
	dc->Draw(4, 0);

	dc->VSSetShader(NULL, 0, 0);
	dc->PSSetShader(NULL, 0, 0);
	dc->PSSetConstantBuffers(CBIndex, 1, cacheBuffer.GetAddressOf());
}

void SkyMap::DrawGui()
{
#if USE_IMGUI
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(u8"デバッグ"))
		{
			if (ImGui::BeginMenu(u8"描画管理"))
			{
				if (ImGui::BeginMenu(u8"スカイマップ"))
				{
					ImGui::SliderInt(u8"ルックアップテーブル番号", &_lutIndex, 0, LutMaxIndex - 1);
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
#endif
}
