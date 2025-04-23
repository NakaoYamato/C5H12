#pragma once

#include "../Actor.h"
#include "../../Library/Camera/Camera.h"

class MainCamera : public Actor
{
public:
	~MainCamera()override {}

	// 生成時処理
	void OnCreate() override;

	// 開始関数
	void Start() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// GUI描画
	void DrawGui() override;

	// 指定方向を向く
	void SetLookAt(const Vector3& eye, const Vector3& focus, const Vector3& up);

	// パースペクティブ設定
	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);
};