#pragma once

#include "../../Library/Component/Component.h"

#include "../../Library/Model/Model.h"

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
	void StartShake(float duration, float magnitude);

private:
	std::weak_ptr<Model> _model;
	// 揺らすノードの番号
	int _nodeIndex = 0;
	// シェイク方向
	Vector3 _shakeDirection = Vector3::Up;
	// 元の位置
	Vector3 _originalPosition;
	// シェイク時間
	float _shakeDuration = 0.0f;
	// シェイクマグニチュード
	float _shakeMagnitude = 0.0f;
	// シェイク経過時間
	float _shakeElapsed = 0.0f;
};