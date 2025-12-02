#include "PlayerInput.h"

#include "../../Library/Scene/Scene.h"
#include <imgui.h>

// 開始処理
void PlayerInput::Start()
{
	_playerController = GetActor()->GetComponent<PlayerController>();
	_playerItemController = GetActor()->GetComponent<PlayerItemController>();
}

// 更新処理
void PlayerInput::OnUpdate(float elapsedTime)
{
	auto playerController = _playerController.lock();
	if (playerController == nullptr)
		return;
	auto playerItemController = _playerItemController.lock();
	if (playerItemController == nullptr)
		return;

	// 入力処理
	{
		Vector2 lAxisValue = Vector2(_INPUT_VALUE("AxisLX"), _INPUT_VALUE("AxisLY"));

		// カメラの正面方向と右方向を取得
		Vector3 frontVec = GetActor()->GetScene()->GetMainCamera()->GetFront();
		Vector3 rightVec = GetActor()->GetScene()->GetMainCamera()->GetRight();

		// Y軸の成分を0にして正規化
		frontVec.y = 0.0f;
		rightVec.y = 0.0f;
		frontVec = Vector3::Normalize(frontVec);
		rightVec = Vector3::Normalize(rightVec);

		// 移動ベクトルを計算
		Vector2 movement = {};
		movement.x = frontVec.x * lAxisValue.y + rightVec.x * lAxisValue.x;
		movement.y = frontVec.z * lAxisValue.y + rightVec.z * lAxisValue.x;
		playerController->SetMovement(movement);

        // 移動量があれば移動中とする
		playerController->SetIsMoving(movement.LengthSq() > 0.0f);
	}

	playerController->SetIsDash(_INPUT_PRESSED("Dash"));
	playerController->SetIsGuard(_INPUT_PRESSED("Guard") || _INPUT_VALUE("Guard") > 0.0f);

	playerController->SetIsAttack(_INPUT_TRIGGERD("Action1"));
	playerController->SetIsHoldingAttackKey(_INPUT_PRESSED("Action1"));
	playerController->SetIsSpecialAttack(_INPUT_TRIGGERD("Action2"));
	playerController->SetIsEvade(_INPUT_TRIGGERD("Evade"));

	// アイテムスライダー処理
	if (_INPUT_PRESSED("ItemSelect"))
	{
		if (playerItemController->IsClosed())
		{
			playerItemController->Open();
		}

		if (playerItemController->IsOpen())
		{
			if (_INPUT_TRIGGERD("ItemPrevSlide"))
				playerItemController->AddIndex(-1);
			if (_INPUT_TRIGGERD("ItemNextSlide"))
				playerItemController->AddIndex(+1);
		}
	}
	else
	{
		if (playerItemController->IsOpen())
		{
			playerItemController->Close();
		}

		playerController->SetIsUsingItem(_INPUT_TRIGGERD("Use"));
		playerController->SetIsSelect(_INPUT_TRIGGERD("Select"));
	}

	// マウスホイールによるアイテム選択
	float mouseWheel = _INPUT_VALUE("MouseOldWheel");
	if (mouseWheel > 0.0f)
	{
		playerItemController->AddIndex(-1);
	}
	else if (mouseWheel < 0.0f)
	{
		playerItemController->AddIndex(1);
	}

	// メニュー画面起動入力
	if (_INPUT_TRIGGERD("Menu"))
	{
		_inputManager->SwitchInput("MenuInput");
	}
}

// GUI描画
void PlayerInput::DrawGui()
{
}

// 終了時処理
void PlayerInput::OnExit()
{
	auto playerController = _playerController.lock();
	if (playerController == nullptr)
		return;

	// 入力状態リセット
	playerController->SetMovement(Vector2::Zero);
	playerController->SetIsMoving(false);
	playerController->SetIsDash(false);
	playerController->SetIsGuard(false);

	playerController->SetIsAttack(false);
	playerController->SetIsHoldingAttackKey(false);
	playerController->SetIsSpecialAttack(false);
	playerController->SetIsEvade(false);

	playerController->SetIsUsingItem(false);
	playerController->SetIsSelect(false);
}
