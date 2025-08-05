#pragma once

#include "../../Source/Network/NetworkSender.h"
#include "../../Library/Component/StateController.h"
#include "../../Source/Enemy/EnemyController.h"
#include "../../Source/Common/Damageable.h"

class EnemyNetworkSender : public NetworkSender
{
public:
	EnemyNetworkSender() {}
	~EnemyNetworkSender() override {}
	const char* GetName() const override { return "EnemyNetworkSender"; }
	void Start() override;
	/// <summary>
	/// 敵の移動データを取得
	/// </summary>
	/// <returns></returns>
	Network::CharacterMove GetMoveData() override;
	/// <summary>
	/// 敵のダメージ適用データを取得
	/// </summary>
	/// <returns></returns>
	Network::CharacterApplyDamage GetApplyDamageData() override;
private:
	std::weak_ptr<EnemyController> _enemyController; // 敵コントローラーへの参照
	std::weak_ptr<StateController> _stateController; // ステートコントローラーへの参照
	std::weak_ptr<Damageable> _damageable; // ダメージ可能コンポーネントへの参照
};