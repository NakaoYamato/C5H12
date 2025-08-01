#include "Decal.h"

#include "../../Library/Graphics/GpuResourceManager.h"

#include <Mygui.h>

Decal::Decal(ID3D11Device* device, const wchar_t* colorTextureFilename, const wchar_t* normalTextureFilename)
{
	GpuResourceManager::LoadTextureFromFile(
		device,
		colorTextureFilename,
		_colorSRV.ReleaseAndGetAddressOf(),
		nullptr
	);
	GpuResourceManager::LoadTextureFromFile(
		device,
		normalTextureFilename,
		_normalSRV.ReleaseAndGetAddressOf(),
		nullptr
	);
}

void Decal::DrawGui(ID3D11Device* device)
{
	std::wstring resultPath{};
	if (ImGui::ImageEditButton(&resultPath, _colorSRV.Get()))
	{
		// テクスチャの読み込み
		GpuResourceManager::LoadTextureFromFile(
			device,
			resultPath.c_str(),
			_colorSRV.ReleaseAndGetAddressOf(),
			nullptr);
	}
	if (ImGui::ImageEditButton(&resultPath, _normalSRV.Get()))
	{
		// テクスチャの読み込み
		GpuResourceManager::LoadTextureFromFile(
			device,
			resultPath.c_str(),
			_normalSRV.ReleaseAndGetAddressOf(),
			nullptr);
	}
	ImGui::ColorEdit4("Color", &_color.x);
}
