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

	/// <summary>
	/// 生成時処理
	/// </summary>
	virtual void OnCreate() {}
	/// <summary>
	/// 削除時処理
	/// </summary>
	virtual void OnDelete() {}
	/// <summary>
	/// 開始時処理
	/// </summary>
	virtual void Start() {}
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="elapsedTime"></param>
	virtual void Update(float elapsedTime) {}
	/// <summary>
	/// Update後更新処理
	/// </summary>
	/// <param name="elapsedTime"></param>
	virtual void LateUpdate(float elapsedTime) {}
	/// <summary>
	/// 一定間隔更新処理
	/// </summary>
	virtual void FixedUpdate() {}
	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="rc"></param>
	virtual void Render(const RenderContext& rc) {}
	/// <summary>
	/// デバッグ表示
	/// </summary>
	/// <param name="rc"></param>
	virtual void DebugRender(const RenderContext& rc) {}
	/// <summary>
	/// 影描画
	/// </summary>
	/// <param name="rc"></param>
	virtual void CastShadow(const RenderContext& rc) {}
	/// <summary>
	/// 3D描画後の描画処理
	/// </summary>
	/// <param name="rc"></param>
	/// <summary>
	virtual void DelayedRender(const RenderContext& rc) {}
	/// <summary>
	/// オブジェクトとの接触時の処理
	/// </summary>
	/// <param name="collisionData">接触情報</param>
	virtual void OnContact(CollisionData& collisionData) {}
	/// <summary>
	/// オブジェクトとの接触した瞬間時の処理
	/// </summary>
	/// <param name="collisionData">接触情報</param>
	virtual void OnContactEnter(CollisionData& collisionData) {}
	/// <summary>
	/// 起動フラグが変化したときの処理
	/// </summary>
	/// <param name="isActive">変化後の値</param>
	virtual void OnChangedActive(bool isActive) {}
	/// <summary>
	/// GUI描画
	/// </summary>
	virtual void DrawGui() {}

	// アクター設定
	void SetActor(std::shared_ptr<Actor> actor) { this->_actor = actor; }

	// アクター取得
	std::shared_ptr<Actor> GetActor() { return _actor.lock(); }

private:
	std::weak_ptr<Actor>	_actor;
};