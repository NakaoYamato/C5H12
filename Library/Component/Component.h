#pragma once

#include "../Actor/Actor.h"

/// <summary>
/// コンポーネント基底クラス
/// </summary>
class Component
{
public:
	Component() {}
	virtual ~Component() {}

	// 名前取得
	virtual const char* GetName() const = 0;

	// 開始処理
	virtual void Start() {}

	/// <summary>
	/// 削除時処理
	/// </summary>
	virtual void OnDestroy() {}

	// 更新処理
	virtual void Update(float elapsedTime) {}

	/// 一定間隔の更新処理
	virtual void FixedUpdate() {}

	// 描画処理
	virtual void Render(const RenderContext& rc) {}

	// デバッグ表示
	virtual void DebugRender(const RenderContext& rc) {}

	// 影描画
	virtual void CastShadow(const RenderContext& rc) {}

	// 3D描画後の描画処理
	virtual void DelayedRender(const RenderContext& rc) {}

	/// <summary>
	/// オブジェクトとの接触時の処理
	/// </summary>
	/// <param name="other"></param>
	/// <param name="hitPosition">当たったワールド座標</param>
	/// <param name="hitNormal">接触面のotherからthisに向かう法線</param>
	/// <param name="penetration">めり込み量</param>
	virtual void OnCollision(Actor* other,
		const Vector3& hitPosition,
		const Vector3& hitNormal,
		const float& penetration) {}

	// GUI描画
	virtual void DrawGui() {}

	// アクター設定
	void SetActor(std::shared_ptr<Actor> actor) { this->_actor = actor; }

	// アクター取得
	std::shared_ptr<Actor> GetActor() { return _actor.lock(); }

private:
	std::weak_ptr<Actor>	_actor;
};