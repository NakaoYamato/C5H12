#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

#include "../../Library/Math/Transform.h"
#include "../../Library/Graphics/RenderContext.h"

// 前方宣言
class Component;
class ColliderComponent;
enum class ActorTag;

/// <summary>
/// ゲームオブジェクトの基底クラス
/// </summary>
class Actor : public std::enable_shared_from_this<Actor>
{
public:
	Actor() {}
	virtual ~Actor() {};

	// 開始処理
	virtual void Start();

	// 更新処理
	virtual void Update(float elapsedTime);

	// 1秒ごとの更新処理
	virtual void FixedUpdate();

	// 描画の前処理
	virtual void RenderPreprocess(RenderContext& rc);

	// 描画処理
	virtual void Render(const RenderContext& rc);

	// デバッグ表示
	virtual void DebugRender(const RenderContext& rc);

	// 影描画
	virtual void CastShadow(const RenderContext& rc);

	// 3D描画後の描画処理
	virtual void DelayedRender(const RenderContext& rc);

	// Gui描画
	virtual void DrawGui();

	// 当たり判定処理
	virtual void Judge(Actor* other);

	// 接触時の処理
	virtual void OnCollision(Actor* other, 
		const Vector3& hitPosition, 
		const Vector3& hitNormal, 
		const float& penetration);

#pragma region コンポーネント関係
	// コンポーネント追加
	template<class T, class... Args>
	std::shared_ptr<T> AddComponent(Args... args)
	{
		std::shared_ptr<T> component = std::make_shared<T>(args...);
		component->SetActor(shared_from_this());
		components_.emplace_back(component);
		return component;
	}

	// コンポーネント取得
	template<class T>
	std::shared_ptr<T> GetComponent()
	{
		for (std::shared_ptr<Component>& component : components_)
		{
			std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(component);
			if (p == nullptr) continue;
			return p;
		}
		return nullptr;
	}

	// 当たり判定コンポーネント
	template<class T, class... Args>
	std::shared_ptr<T> AddCollider(Args... args)
	{
		std::shared_ptr<T> component = std::make_shared<T>(args...);
		component->SetActor(shared_from_this());
		colliderComponents_.emplace_back(component);
		return component;
	}
	template<class T>
	std::shared_ptr<T> GetCollider()
	{
		for (std::shared_ptr<ColliderComponent>& component : colliderComponents_)
		{
			std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(component);
			if (p == nullptr) continue;
			return p;
		}
		return nullptr;
	}
	std::shared_ptr<ColliderComponent> GetCollider(size_t index)
	{
		return colliderComponents_[index];
	}
	size_t GetColliderComponentSize()const { return colliderComponents_.size(); }
#pragma endregion


#pragma region アクセサ
	std::weak_ptr<Actor> GetParent() { return parent_; }
	std::vector<std::weak_ptr<Actor>> GetChildren() { return children_; };
	const char* GetName() const { return name_.c_str(); }
	Transform& GetTransform() { return transform_; }
	const std::unordered_map<ActorTag, bool>& GetJudgeTags()const { return judgeTags_; }

	void SetParent(std::shared_ptr<Actor> parent) {
		this->parent_ = parent;
		parent->children_.push_back(shared_from_this());
	}
	void SetName(const char* name) { this->name_ = name; }
	void SetTransform(const Transform& t) { this->transform_ = t; }
	void SetActiveFlag(bool b) { this->isActive_ = b; }
	void SetShowFlag(bool b) { this->isShowing_ = b; }
	void SetDrawDebugFlag(bool b) { this->drawDebug_ = b; }
	// そのタグに対して当たり判定を行うかのフラグをセット
	void SetJudgeTagFlag(ActorTag tag, bool f) { judgeTags_[tag] = f; }

	bool IsActive()const { return isActive_; }
	bool IsShowing()const { return isShowing_; }
	bool DrawDebug()const { return drawDebug_; }

#pragma endregion
protected:
	std::weak_ptr<Actor>	parent_;
	std::vector<std::weak_ptr<Actor>> children_;

	std::string			name_;
	Transform			transform_;

	bool				isActive_ = true;
	bool				isShowing_ = true;
	bool				drawDebug_ = true;

	std::vector<std::shared_ptr<Component>>	components_;
	// 当たり判定コンポーネント
	std::vector<std::shared_ptr<ColliderComponent>>	colliderComponents_;

	// 各タグに対して当たり判定を行うかのフラグ
	std::unordered_map<ActorTag, bool> judgeTags_;
};