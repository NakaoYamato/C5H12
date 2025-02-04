#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>

#include "../Math/Vector.h"
#include "../Math/Quaternion.h"

namespace ShapeRenderer
{
	void Initialize(ID3D11Device* device);

	// î†ï`âÊ
	void DrawBox(
		const Vector3& position,
		const Vector3& angle,
		const Vector3& size,
		const Vector4& color = VECTOR4_WHITE);
	void DrawBox(
		const Vector3& position,
		const Quaternion& oriental,
		const Vector3& size,
		const Vector4& color = VECTOR4_WHITE);
	void DrawBox(
		const DirectX::XMFLOAT4X4& matrix,
		const Vector4& color = VECTOR4_WHITE);

	// ãÖï`âÊ
	void DrawSphere(
		const Vector3& position,
		float radius,
		const Vector4& color = VECTOR4_WHITE);
	void DrawSphere(
		const DirectX::XMVECTOR& position,
		float radius,
		const Vector4& color = VECTOR4_WHITE);

	// ÉJÉvÉZÉãï`âÊ
	void DrawCapsule(
		const DirectX::XMFLOAT4X4& transform,
		float radius,
		float height,
		const Vector4& color = VECTOR4_WHITE);

	// ï`âÊé¿çs
	void Render(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection);

	// TODO:
	// âeï`âÊ

}