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
class Scene;

/// <summary>
/// ゲームオブジェクトの基底クラス
/// </summary>
class Actor : public std::enable_shared_from_this<Actor>
{
public:
	Actor() {}
	virtual ~Actor() {};

	// 生成時処理
	virtual void OnCreate() {};

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
		_components.emplace_back(component);
		return component;
	}

	// コンポーネント取得
	template<class T>
	std::shared_ptr<T> GetComponent()
	{
		for (std::shared_ptr<Component>& component : _components)
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
		_colliders.emplace_back(component);
		return component;
	}
	template<class T>
	std::shared_ptr<T> GetCollider()
	{
		for (std::shared_ptr<ColliderComponent>& component : _colliders)
		{
			std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(component);
			if (p == nullptr) continue;
			return p;
		}
		return nullptr;
	}
	std::shared_ptr<ColliderComponent> GetCollider(size_t index)
	{
		return _colliders[index];
	}
	size_t GetColliderComponentSize()const { return _colliders.size(); }
#pragma endregion


#pragma region アクセサ
	const char* GetName() const { return _name.c_str(); }
	Transform& GetTransform() { return _transform; }
	const std::unordered_map<ActorTag, bool>& GetJudgeTags()const { return _judgeTags; }

	void SetScene(Scene* scene) { this->_scene = scene; }
	void SetName(const char* name) { this->_name = name; }
	void SetTransform(const Transform& t) { this->_transform = t; }
	void SetActiveFlag(bool b) { this->_isActive = b; }
	void SetShowFlag(bool b) { this->_isShowing = b; }
	void SetDrawDebugFlag(bool b) { this->_drawDebug = b; }
	// そのタグに対して当たり判定を行うかのフラグをセット
	void SetJudgeTagFlag(ActorTag tag, bool f) { _judgeTags[tag] = f; }

	bool IsActive()const { return _isActive; }
	bool IsShowing()const { return _isShowing; }
	bool DrawDebug()const { return _drawDebug; }

#pragma endregion
protected:
	// トランスフォーム更新
	virtual void UpdateTransform();

protected:
	Scene* _scene = nullptr;

	std::string			_name;
	Transform			_transform;

	bool				_isActive = true;
	bool				_isShowing = true;
	bool				_drawDebug = true;
	bool				_useGuizmo = true;

	std::vector<std::shared_ptr<Component>>	_components;
	// 当たり判定コンポーネント
	std::vector<std::shared_ptr<ColliderComponent>>	_colliders;

	// 各タグに対して当たり判定を行うかのフラグ
	std::unordered_map<ActorTag, bool> _judgeTags;
};