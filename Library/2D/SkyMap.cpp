#include "SkyMap.h"
#include "../Graphics/GpuResourceManager.h"

#include "../HRTrace.h"

#include <imgui.h>

SkyMap::SkyMap(ID3D11Device* device, const wchar_t* filename, const wchar_t* diffuseIEM, const wchar_t* specularIDM)
{
	// スカイマップテクスチャ読み込み
	_shaderResourceView.Load(device, filename);
	// テクスチャがキューブマップかどうか確認
	if (_shaderResourceView.GetTexture2DDesc().MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
	{
		_isTextureCube = true;
	}

	// シェーダー読み込み
	_vertexShader.Load(device, "./Data/Shader/HLSL/SkyMap/SkyMapVS.cso", nullptr, 0);
	_pixelShader[0].Load(device, "./Data/Shader/HLSL/SkyMap/SkyMapPS.cso");
	_pixelShader[1].Load(device, "./Data/Shader/HLSL/SkyMap/SkyBoxPS.cso");

	if (diffuseIEM)
		_diffuseIEMSRV.Load(device, diffuseIEM);
	if (specularIDM)
		_specularIEMSRV.Load(device, specularIDM);

	// LUTテクスチャ読み込み
	_lutSRVs[0].Load(device, L"./Data/SkyMap/lut_ggx.DDS");
	_lutSRVs[1].Load(device, L"./Data/SkyMap/lut_charlie.DDS");
	_lutSRVs[2].Load(device, L"./Data/SkyMap/lut_sheen_E.DDS");

	// 定数バッファ作成
	_constantBuffer.Create(device, sizeof(Constants));
}

// 描画
void SkyMap::Blit(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// 現在使用している定数バッファのキャッシュを保存
	static constexpr int CBIndex = 0;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cacheBuffer;
	dc->PSGetConstantBuffers(CBIndex, 1, cacheBuffer.ReleaseAndGetAddressOf());

	// シェーダ設定
	dc->IASetInputLayout(NULL);
	dc->VSSetShader(_vertexShader.Get(), 0, 0);
	dc->PSSetShader(_isTextureCube ? _pixelShader[1].Get() : _pixelShader[0].Get(), 0, 0);

	dc->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ設定
	dc->PSSetShaderResources(0, 1, _shaderResourceView.GetAddressOf());

	// 定数バッファ更新
	DirectX::XMMATRIX VP =
		DirectX::XMLoadFloat4x4(&rc.camera->GetView()) *
		DirectX::XMLoadFloat4x4(&rc.camera->GetProjection());
	Constants data{};
	DirectX::XMStoreFloat4x4(&data.inverseViewProjection, DirectX::XMMatrixInverse(NULL, VP));
	_constantBuffer.Update(dc, &data);
	dc->PSSetConstantBuffers(CBIndex, 1, _constantBuffer.GetAddressOf());

	// 描画
	dc->Draw(4, 0);

	// シェーダ解除
	dc->VSSetShader(NULL, 0, 0);
	dc->PSSetShader(NULL, 0, 0);
	// 定数バッファ復元
	dc->PSSetConstantBuffers(CBIndex, 1, cacheBuffer.GetAddressOf());
}

// GUI描画
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
