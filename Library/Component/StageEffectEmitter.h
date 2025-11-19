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
	// オブジェクトとの接触した瞬間時の処理
	void OnContactEnter(CollisionData& collisionData) override;

	void SetEffectIndex(int index) { _effectIndex = index; }
private:
	std::weak_ptr<EffectController> _effectController;
	int _effectIndex = 0;

	// カメラシェイクパラメータ
	float _amplitude = 0.1f;
	float _frequency = 10.0f;
	float _duration = 0.5f;
	Vector3 _directionMask = { 1,1,1 };
	ShakeDecayType _decayType = ShakeDecayType::Linear;
};