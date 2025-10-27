#pragma once

#include "../../Library/Component/Component.h"

#include "../../Library/Model/Model.h"
#include "Damageable.h"

class NodeShaker : public Component
{
public:
	NodeShaker() {}
	virtual ~NodeShaker() override = default;
	// 名前取得
	const char* GetName() const override { return "NodeShaker"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void LateUpdate(float elapsedTime) override;
	// Gui描画
	void DrawGui() override;
	// シェイク開始
	void StartShake(const Vector3& position, float duration, float magnitude);

#pragma region アクセサ
	// モデル設定
    void SetModel(std::shared_ptr<Model> model) { _model = model; }
    // 揺らすノードの番号設定
    void SetNodeIndex(int index) { _nodeIndex = index; }
	// シェイク処理タイプ設定
    void SetShakeEasingType(EasingType type) { _shakeEasingType = type; }

	// ダメージによるシェイク時間取得
	float GetDamageShakeDuration() const { return _damageShakeDuration; }
	// ダメージによるシェイクマグニチュード取得
	float GetDamageShakeMagnitude() const { return _damageShakeMagnitude; }
	// ダメージによるシェイク時間設定
	void SetDamageShakeDuration(float duration) { _damageShakeDuration = duration; }
	// ダメージによるシェイクマグニチュード設定
    void SetDamageShakeMagnitude(float magnitude) { _damageShakeMagnitude = magnitude; }
#pragma endregion

private:
	std::weak_ptr<Model> _model;
	// 揺らすノードの番号
	int _nodeIndex = 0;
	// シェイク方向
	Vector3 _shakeDirection = Vector3::Up;
	// シェイク時間
	float _shakeDuration = 0.0f;
	// シェイクマグニチュード
	float _shakeMagnitude = 0.0f;
	// シェイク経過時間
	float _shakeElapsed = 0.0f;
	// シェイク処理タイプ
    EasingType _shakeEasingType = EasingType::InOutSine;

	// ダメージによるシェイク時間
    float _damageShakeDuration = 0.5f;
    // ダメージによるシェイクマグニチュード
    float _damageShakeMagnitude = 10.0f;

	// 元のクォータニオン
    Quaternion _originalRotation;
	// 計算後のクォータニオン
    Quaternion _shakenRotation;
};