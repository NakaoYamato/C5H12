#pragma once

#include "ColliderBase.h"
#include "../../Library/Model/ModelCollision.h"
#include "../Animator.h"
#include "../../Library/Scene/Scene.h"

class ModelCollider : public ColliderBase
{
public:
	ModelCollider() {}
	~ModelCollider() override {}
	// 名前取得
	const char* GetName() const override { return "ModelCollider"; }
	// 生成時処理
	void OnCreate() override;
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// デバッグ描画処理
	void DebugRender(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;

	// モデル当たり判定情報取得
	const ModelCollision& GetModelCollision() const { return _modelCollision; }
	bool IsCollAttackEvent() const { return _collAttackEvent; }
private:
	// アニメーションイベントの当たり判定更新
	// sphereDatas : アニメーションイベントの球体当たり判定データ
	// capsuleDatas : アニメーションイベントのカプセル当たり判定データ
	void UpdateAnimationEventCollision(
		float elapsedTime, 
		std::vector<CollisionManager::SphereData>& sphereDatas,
		std::vector<CollisionManager::CapsuleData>& capsuleDatas);
	// モデルの球体当たり判定がアニメーションイベントの当たり判定と接触しているか確認
	bool IntersectAnimationEvent(
		const Vector3& modelSpherePosition,
		float modelSphereRadius,
		const std::vector<CollisionManager::SphereData>& eventSphereDatas,
		const std::vector<CollisionManager::CapsuleData>& eventCapsuleDatas) const;
	// モデルのカプセル当たり判定がアニメーションイベントの当たり判定と接触しているか確認
	bool IntersectAnimationEvent(
		const Vector3& modelCapsuleStart,
		const Vector3& modelCapsuleEnd,
		float modelCapsuleRadius,
		const std::vector<CollisionManager::SphereData>& eventSphereDatas,
		const std::vector<CollisionManager::CapsuleData>& eventCapsuleDatas) const;
private:
	// モデル当たり判定情報
	ModelCollision _modelCollision;
	// 当たり判定タグごとのアクター
	std::unordered_map<std::string, std::shared_ptr<Actor>> _tagActors;
	// アニメータ
	std::weak_ptr<Animator> _animator;
	// 攻撃判定が出たかどうか
	bool _collAttackEvent = false;
};