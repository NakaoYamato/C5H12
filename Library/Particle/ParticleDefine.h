#pragma once

#include "../../Library/Math/Vector.h"
#include "../../Library/Graphics/RenderContext.h"

#include <minwindef.h>

// パーティクル更新タイプ
enum class ParticleUpdateType : UINT
{
	Default = 0,		// 通常
};

// パーティクル描画タイプ
enum class ParticleRenderType : UINT
{
	Default = 0,		// 通常
	Billboard = 1,		// ビルボード
};

// パーティクル生成用構造体
struct ParticleEmitData
{
	// 更新タイプ
	ParticleUpdateType	updateType = ParticleUpdateType::Default;
	// 描画タイプ
	ParticleRenderType	renderType = ParticleRenderType::Default;
	// 生存時間
	float	timer = 0.0f;
	float	padding_0 = 0.0f;

	// 発生位置
	Vector4 position = Vector4::Zero;
	// 開始時の回転(ラジアン)
	Vector4 startRotation = Vector4::Zero;
	// 終了時の回転(ラジアン)
	Vector4 endRotation = Vector4::Zero;
	// 開始時の大きさ
	Vector4 startScale = Vector4::One;
	// 終了時の大きさ
	Vector4 endScale = Vector4::One;
	// 速度
	Vector4 velocity = Vector4::Zero;
	// 加速力
	Vector4 acceleration = Vector4::Zero;
	// 初期色
	Vector4 startColor = Vector4::White;
	// 終了色
	Vector4 endColor = Vector4::White;

	// テクスチャ座標
	Vector2 texPosition = Vector2::Zero;
	// テクスチャの大きさ
	Vector2 texSize = Vector2::One;
	// テクスチャの分割数
	DirectX::XMUINT2 texSplit = DirectX::XMUINT2();
	// テクスチャ切り取り番号
	UINT	texcoordIndex = 0;
	// テクスチャアニメーションの速度
	float	texAnimTime = 0.0f;
};
