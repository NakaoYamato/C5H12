#pragma once

#include "../../Library/Component/CameraControllerBase.h"
#include "../../Source/Player/PlayerActor.h"

class PlayerDeathCamera : public CameraControllerBase
{
public:
	PlayerDeathCamera() {}
	~PlayerDeathCamera() override {}
	// 名前取得
	const char* GetName() const override { return "PlayerDeathCamera"; }
	// 開始時処理
	void OnStart() override {}
	// 起動時関数
	void OnActivate() override;
	// 更新時処理
	void OnUpdate(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

	// プレイヤーアクター設定
	void SetPlayerActor(PlayerActor* playerActor);
	// 次のコントローラー名設定
	void SetNextControllerName(const std::string& name)
	{
		_nextControllerName = name;
	}
private:
	PlayerActor* _playerActor = nullptr;

	float _timer = 0.0f;
	float _duration = 3.0f;

	float _focusVerticalOffset = 1.5f;
	float _cameraDistance = 5.5f;
	float _cameraUp = 1.0f;
	float _cameraUpRate = 1.0f;

	// 遷移先
	std::string _nextControllerName{};
};