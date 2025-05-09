#pragma once

#include <vector>
#include <memory>

#include "../Graphics/RenderContext.h"

#include "../Component/Collider/SphereCollider.h"
#include "../Component/Collider/BoxCollider.h"
#include "../Component/Collider/CapsuleCollider.h"
#include "../Component/Collider/MeshCollider.h"

class CollisionManager
{
public:
	CollisionManager() {}
	~CollisionManager() {}

	// 更新処理
	void Update();

	// デバッグ描画処理
	void DebugRender(const RenderContext& rc);

	// GUI描画処理
	void DrawGui();

public:
#pragma region 登録
	// 球コライダー登録
	void RegisterSphereCollider(SphereCollider* sphereCollider);
	// ボックスコライダー登録	
	void RegisterBoxCollider(BoxCollider* boxCollider);
	// カプセルコライダー登録
	void RegisterCapsuleCollider(CapsuleCollider* capsuleCollider);
	// メッシュコライダー登録
	void RegisterMeshCollider(MeshCollider* meshCollider);
#pragma endregion

#pragma region 削除
	// 球コライダー削除
	void UnregisterSphereCollider(SphereCollider* sphereCollider);
	// ボックスコライダー削除
	void UnregisterBoxCollider(BoxCollider* boxCollider);
	// カプセルコライダー削除
	void UnregisterCapsuleCollider(CapsuleCollider* capsuleCollider);
	// メッシュコライダー削除
	void UnregisterMeshCollider(MeshCollider* meshCollider);
#pragma endregion

private:
	std::vector<SphereCollider*>	_sphereColliders; // 球コライダー
	std::vector<BoxCollider*>		_boxColliders; // ボックスコライダー
	std::vector<CapsuleCollider*>	_capsuleColliders; // カプセルコライダー
	std::vector<MeshCollider*>		_meshColliders; // メッシュコライダー

#pragma region デバッグ用
	bool _isDebugDraw = true; // デバッグ描画フラグ
	bool _isDebugDrawVertex = true; // 頂点描画フラグ
#pragma endregion

};