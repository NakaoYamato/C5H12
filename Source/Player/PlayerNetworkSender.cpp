#include "PlayerNetworkSender.h"

void PlayerNetworkSender::Start()
{
	// プレイヤーコントローラーの取得
	_playerController = GetActor()->GetComponent<PlayerController>();
	// ステートコントローラーの取得
	//_stateController = GetActor()->GetComponent<StateController>();
	// ダメージ可能コンポーネントの取得
	_damageable = GetActor()->GetComponent<Damageable>();
}

Network::CharacterMove PlayerNetworkSender::GetMoveData()
{
	Network::CharacterMove moveData{};

	//auto controller = _playerController.lock();
	//auto state = _stateController.lock();
	//if (!controller || !state)
	//{
	//	moveData.uniqueID = -1; // 無効なユニークID
	//	return moveData;
	//}
	//moveData.uniqueID = 0;

	//moveData.position = GetActor()->GetTransform().GetPosition();
	//Vector3 movement = Vector3(controller->GetMovement().x, 0.0f, controller->GetMovement().y);
	//moveData.target = movement;
	//moveData.angleY = GetActor()->GetTransform().GetAngle().y;
	//strcpy_s(moveData.mainState, state->GetStateName());
	//strcpy_s(moveData.subState, state->GetSubStateName());

	return moveData;
}

Network::CharacterApplyDamage PlayerNetworkSender::GetApplyDamageData()
{
	Network::CharacterApplyDamage damageData{};
	auto damageable = _damageable.lock();
	if (!damageable || damageable->GetLastDamage() <= 0.0f)
	{
		damageData.uniqueID = -1; // 無効なユニークID
		return damageData;
	}
	damageData.uniqueID = 0; // プレイヤーのユニークIDを設定
	damageData.damage = damageable->GetLastDamage();
	damageData.hitPosition = damageable->GetHitPosition();
	return damageData;
}
