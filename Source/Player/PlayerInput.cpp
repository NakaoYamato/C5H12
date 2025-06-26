#include "PlayerInput.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始処理
void PlayerInput::Start()
{
	if (FindInputMediator())
	{
		_inputMediator.lock()->ReceiveCommand(nullptr, InputMediator::CommandType::StartGame, "");
	}

	_stateController = GetActor()->GetComponent<StateController>();
}

// 更新処理
void PlayerInput::Update(float elapsedTime)
{
	// 起動状態でなければ処理しない
	if (!IsActive())
		return;

	if (_stateController.lock() == nullptr)
		return;
	auto stateMachine = std::dynamic_pointer_cast<PlayerStateMachine>(_stateController.lock()->GetStateMachine());
	if (stateMachine == nullptr)
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
		stateMachine->SetMovement(movement);

        // 移動量があれば移動中とする
		stateMachine->SetIsMoving(movement.LengthSq() > 0.0f);
	}

	stateMachine->SetIsDash(_INPUT_PRESSED("Dash"));
	stateMachine->SetIsGuard(_INPUT_PRESSED("Guard") || _INPUT_VALUE("Guard") > 0.0f);

	stateMachine->SetIsAttack(_INPUT_TRIGGERD("Action1"));
	stateMachine->SetIsEvade(_INPUT_TRIGGERD("Evade"));

	// メニュー画面起動入力
	if (_INPUT_TRIGGERD("Menu"))
	{
		if (_inputMediator.lock())
		{
			_inputMediator.lock()->ReceiveCommand(this, InputMediator::CommandType::OpenMenu, "");
		}
	}
}

// GUI描画
void PlayerInput::DrawGui()
{
}

// Mediatorから命令を受信
void PlayerInput::ReceiveCommandFromOther(InputMediator::CommandType commandType, const std::string& command)
{
	switch (commandType)
	{
	case InputMediator::CommandType::StartGame:
		// ゲーム開始の命令を受け取った場合、アクティブにする
		SetActive(true);
		break;
	case InputMediator::CommandType::OpenMenu:
		// メニュー画面を開く命令を受け取った場合、非アクティブにする
		SetActive(false);
		break;
	case InputMediator::CommandType::CloseMenu:
		// メニュー画面を閉じる命令を受け取った場合、アクティブにする
		SetActive(true);
		break;
	default:
		break;
	}
}
