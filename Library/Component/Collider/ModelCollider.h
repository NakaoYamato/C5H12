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

	// 部位ごとのアクター生成
	Actor* CreateTagActor(const std::string& tag)
	{
		auto actor = GetActor()->GetScene()->RegisterActor<Actor>(
			GetActor()->GetName() + tag,
			GetActor()->GetTag()
		);
		actor->SetParent(GetActor().get());
		_bodyPartActors[tag] = actor;
		return actor.get();
	}

	// 部位ごとのアクター取得
	std::unordered_map<std::string, std::weak_ptr<Actor>>& GetTagActors()
	{
		return _bodyPartActors;
	}
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
	// 部位ごとのアクター
	std::unordered_map<std::string, std::weak_ptr<Actor>> _bodyPartActors;
	// アニメータ
	std::weak_ptr<Animator> _animator;
	// 攻撃判定が出たかどうか
	bool _collAttackEvent = false;
};