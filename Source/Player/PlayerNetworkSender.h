#pragma once

#include "../../Source/Network/NetworkSender.h"
#include "../../Library/Component/StateController.h"
#include "../../Source/Player/PlayerController.h"
#include "../../Source/Common/Damageable.h"

class PlayerNetworkSender : public NetworkSender
{
public:
	PlayerNetworkSender() {}
	~PlayerNetworkSender() override {}
	const char* GetName() const override { return "PlayerNetworkSender"; }

	void Start() override;
	/// <summary>
	/// プレイヤーの移動データを取得
	/// </summary>
	/// <returns></returns>
	Network::CharacterMove GetMoveData() override;
	/// <summary>
	/// プレイヤーのダメージ適用データを取得
	/// </summary>
	/// <returns></returns>
	Network::CharacterApplyDamage GetApplyDamageData() override;
private:
	std::weak_ptr<PlayerController> _playerController; // プレイヤーコントローラーへの参照
	std::weak_ptr<StateController> _stateController; // ステートコントローラーへの参照
	std::weak_ptr<Damageable> _damageable; // ダメージ可能コンポーネントへの参照
};