#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "../../Library/Math/Vector.h"

class Decal
{
public:
	Decal(ID3D11Device* device, const wchar_t* colorTextureFilename, const wchar_t* normalTextureFilename);
	~Decal() = default;

	void DrawGui(ID3D11Device* device);
#pragma region アクセサ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetColorSRV() { return _colorSRV; }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetNormalSRV() { return _normalSRV; }
	const Vector4& GetColor() const { return _color; }
	int GetDecalMask() const { return decalMask; }
#pragma endregion

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _colorSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _normalSRV;
	Vector4 _color = Vector4::White; // デカールの色
	int		decalMask = 0;// デカールのマスク
};