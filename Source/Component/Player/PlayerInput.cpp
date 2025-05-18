#include "PlayerInput.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始処理
void PlayerInput::Start()
{
    _playerController = GetActor()->GetComponent<PlayerController>();
}

// 更新処理
void PlayerInput::Update(float elapsedTime)
{
    auto player = _playerController.lock();
	if (player == nullptr)
		return;

	// 入力処理
	{
		Vector2 lAxisValue = Vector2(_INPUT_IS_AXIS("AxisLX"), _INPUT_IS_AXIS("AxisLY"));

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
        player->SetMovement(Vector2::Normalize(movement));

        // 移動量があれば移動中とする
		player->SetIsMoving(movement.LengthSq() > 0.0f);
	}

	player->SetIsDash(_INPUT_IS_PRESSED("Dash"));
    player->SetIsGuard(_INPUT_IS_PRESSED("Guard") || _INPUT_IS_AXIS("Guard") > 0.0f);

    player->SetIsAttack(_INPUT_IS_TRIGGERD("Action1"));
    player->SetIsEvade(_INPUT_IS_TRIGGERD("Evade"));
}

// GUI描画
void PlayerInput::DrawGui()
{
}
