#pragma once

#include "../../Library/Graphics/GpuResourceManager.h"

class ConstantBuffer
{
public:
	ConstantBuffer() = default;
	~ConstantBuffer() = default;
	// 定数バッファの生成
	void Create(ID3D11Device* device, UINT bufferSize);
	// 定数バッファにデータを転送
	void Update(ID3D11DeviceContext* dc, const void* data);
	// 定数バッファを取得
	ID3D11Buffer* Get() { return _constantBuffer.Get(); }
	// 定数バッファを取得
	ID3D11Buffer** GetAddressOf() { return _constantBuffer.GetAddressOf(); }

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>		_constantBuffer;
};