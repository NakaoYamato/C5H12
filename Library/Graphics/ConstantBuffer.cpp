#include "ConstantBuffer.h"

// 定数バッファの生成
void ConstantBuffer::Create(ID3D11Device* device, UINT bufferSize)
{
	(void)GpuResourceManager::CreateConstantBuffer(
		device,
		bufferSize,
		_constantBuffer.ReleaseAndGetAddressOf());
}

// 定数バッファにデータを転送
void ConstantBuffer::Update(ID3D11DeviceContext* dc, const void* data)
{
	if (_constantBuffer)
		dc->UpdateSubresource(_constantBuffer.Get(), 0, 0, data, 0, 0);
}
