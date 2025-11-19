#pragma once

#include "Animator.h"

class AnimatorCamera : public Component
{
public:
	AnimatorCamera() = default;
	~AnimatorCamera() override = default;
	// 名前取得
	const char* GetName() const override { return "AnimatorCamera"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

private:
	std::weak_ptr<Animator> _animator;
};