#include "EnvironmentDestroyer.h"

// TODO : 修正
//#include "../../Library/Component/Terrain/TerrainEnvironmentController.h"

// オブジェクトとの接触した瞬間時の処理
void EnvironmentDestroyer::OnContactEnter(CollisionData& collisionData)
{
	//auto environment = collisionData.other->GetComponent<TerrainEnvironmentController>();
	//if (environment && !collisionData.otherIsTrigger)
	//{
	//	Debug::Output::String(L"地形オブジェクトに接触\n");
	//	collisionData.other->Remove();
	//	return;
	//}
}
