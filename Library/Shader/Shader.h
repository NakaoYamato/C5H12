#pragma once

#include "../../Library/Graphics/GpuResourceManager.h"

class PixelShader
{
public:
	PixelShader() = default;
	~PixelShader() = default;

	void Load(ID3D11Device* device, const std::string& filepath)
	{
		_filepath = filepath;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
		GpuResourceManager::CreatePsFromCso(
			device,
			_filepath.c_str(),
			pixelShader.ReleaseAndGetAddressOf()
		);
	}

	ID3D11PixelShader* Get()
	{
		return GpuResourceManager::GetPixelShader(_filepath).Get();
	}

private:
	std::string _filepath{};
};