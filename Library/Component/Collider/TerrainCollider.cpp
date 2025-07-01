#include "TerrainCollider.h"

#include "../../Scene/Scene.h"
#include "../../DebugSupporter/DebugSupporter.h"

#include <imgui.h>

// 開始処理
void TerrainCollider::Start()
{
	// コライダーの登録
	GetActor()->GetScene()->GetCollisionManager().RegisterMeshCollider(this);
	_recalculate = true;
}
// コリジョンメッシュの再計算
void TerrainCollider::RecalculateCollisionMesh()
{

}
