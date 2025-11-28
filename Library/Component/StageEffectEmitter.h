#pragma once

#include "EffectController.h"
#include "../../Library/Scene/Scene.h"

class StageEffectEmitter : public Component
{
public:
	StageEffectEmitter() {}
	~StageEffectEmitter() override {}
	// 名前取得
	const char* GetName() const override { return "StageEffectEmitter"; }
	// 開始時処理
	void Start() override;
	// GUI描画
	void DrawGui() override;

	// エフェクト再生
	// effectScale : 効果の大きさ(1.0fで通常サイズ)
	// shakePower : カメラシェイクの強さ(1.0fで通常サイズ)
	void PlayEffect(const Vector3& position, const Vector3& angle, float effectScale, float shakePower);

	// 効果音インデックス設定
	void SetEffectIndex(int index) { _effectIndex = index; }
private:
	std::weak_ptr<EffectController> _effectController;
	int _effectIndex = 0;
	Vector3 _effectStartScale = Vector3::One;

	// カメラシェイクパラメータ
	float _amplitude = 0.1f;
	float _frequency = 10.0f;
	float _duration = 0.5f;
	Vector3 _directionMask = { 1,1,1 };
	ShakeDecayType _decayType = ShakeDecayType::Linear;
};