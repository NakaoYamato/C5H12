#include "EnemyNetworkSender.h"

void EnemyNetworkSender::Start()
{
	// 敵コントローラーの取得
	_enemyController = GetActor()->GetComponent<EnemyController>();
	// ステートコントローラーの取得
	//_stateController = GetActor()->GetComponent<StateController>();
	// ダメージ可能コンポーネントの取得
	_damageable = GetActor()->GetComponent<Damageable>();
	// 戦闘状態コントローラーの取得
	_combatStatus = GetActor()->GetComponent<CombatStatusController>();
}

Network::CharacterMove EnemyNetworkSender::GetMoveData()
{
	Network::CharacterMove moveData{};
	//auto controller = _enemyController.lock();
	//auto state = _stateController.lock();
	//auto combatStatus = _combatStatus.lock();
	//if (!controller || !state || !combatStatus)
	//{
	//	moveData.uniqueID = -1; // 無効なユニークID
	//	return moveData;
	//}
	//moveData.uniqueID = 0;
	//moveData.position = GetActor()->GetTransform().GetPosition();
	//moveData.target = combatStatus->GetTargetPosition();
	//moveData.angleY = GetActor()->GetTransform().GetAngle().y;
	//strcpy_s(moveData.mainState, state->GetStateName());
	//strcpy_s(moveData.subState, state->GetSubStateName());
	return moveData;
}

Network::CharacterApplyDamage EnemyNetworkSender::GetApplyDamageData()
{
	Network::CharacterApplyDamage damageData{};
	auto damageable = _damageable.lock();
	if (!damageable || damageable->GetLastDamage() <= 0.0f)
	{
		damageData.uniqueID = -1; // 無効なユニークID
		return damageData;
	}
	damageData.uniqueID = 0;
	damageData.damage = damageable->GetLastDamage();
	damageData.hitPosition = damageable->GetHitPosition();
	return damageData;
}
