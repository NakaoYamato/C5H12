#pragma once

#include <vector>
#include <memory>

#include "../Actor/Actor.h"
#include "../Network/ServerAssignment.h"
#include "../Network/ClientAssignment.h"

/// <summary>
/// ネットワーク処理の仲介者
/// </summary>
class NetworkMediator
{
public:
	NetworkMediator() = default;
	~NetworkMediator() = default;

private:
	std::shared_ptr<ServerAssignment> _server;
};
