#pragma once

#include "../../Library/Actor/Actor.h"
#include "../../Library/Component/Network/ClientConnection.h" 
#include "../../Source/Mediator/NetworkMediator.h"

class GameSystemActor : public Actor
{
public:
	~GameSystemActor()override {}

	// 開始時処理
	void OnCreate() override;

	// 開始時処理
	void Start() override;

	// GUI描画
	void DrawGui() override;
private:
    std::weak_ptr<ClientConnection> _clientConnection;
    NetworkMediator _networkMediator;
};