#include "SkyMap.h"
#include "../ResourceManager/GpuResourceManager.h"

#include "../HRTrace.h"

SkyMap::SkyMap(ID3D11Device* device, const wchar_t* filename)
{
	D3D11_TEXTURE2D_DESC texture2d_desc;
	GpuResourceManager::LoadTextureFromFile(device, filename, shaderResourceView.GetAddressOf(), &texture2d_desc);

	if (texture2d_desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
	{
		isTextureCube = true;
	}

	GpuResourceManager::CreateVsFromCso(device, ".\\Data\\Shader\\SkyMapVS.cso", vertexShader.GetAddressOf(), NULL, NULL, 0);
	GpuResourceManager::CreatePsFromCso(device, ".\\Data\\Shader\\SkyMapPS.cso", pixelShader[0].GetAddressOf());
	GpuResourceManager::CreatePsFromCso(device, ".\\Data\\Shader\\SkyBoxPS.cso", pixelShader[1].GetAddressOf());

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(Constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&buffer_desc, nullptr, constantBuffer.GetAddressOf());
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

	dc->VSSetShader(vertexShader.Get(), 0, 0);
	dc->PSSetShader(isTextureCube ? pixelShader[1].Get() : pixelShader[0].Get(), 0, 0);

	dc->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());

	DirectX::XMMATRIX        VP = DirectX::XMLoadFloat4x4(&rc.camera->view_) *
		DirectX::XMLoadFloat4x4(&rc.camera->projection_);
	Constants data;
	DirectX::XMStoreFloat4x4(&data.inverseViewProjection, DirectX::XMMatrixInverse(NULL, VP));

	dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
	dc->PSSetConstantBuffers(CBIndex, 1, constantBuffer.GetAddressOf());
	dc->Draw(4, 0);

	dc->VSSetShader(NULL, 0, 0);
	dc->PSSetShader(NULL, 0, 0);
	dc->PSSetConstantBuffers(CBIndex, 1, cacheBuffer.GetAddressOf());
}
