#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

#include "../../Library/Math/Transform.h"
#include "../../Library/Graphics/RenderContext.h"
#include "../../Library/3D/Model.h"

// 前方宣言
class Component;
class ColliderBaseComponent;
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

	/// <summary>
	/// 生成時処理
	/// </summary>
	virtual void OnCreate() {};

	/// <summary>
	// 開始処理
	/// </summary>
	virtual void Start();

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="elapsedTime">前フレームからの更新時間</param>
	virtual void Update(float elapsedTime);

	/// <summary>
	/// 一定間隔の更新処理
	/// #include "../../Library/Scene/Scene.h"をインクルードして
	/// _FIXED_UPDATE_INTERVAL　が一定間隔(秒)
	/// </summary>
	virtual void FixedUpdate();

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="rc"></param>
	virtual void Render(const RenderContext& rc);

	/// <summary>
	// デバッグ表示
	/// </summary>
	/// <param name="rc"></param>
	virtual void DebugRender(const RenderContext& rc);

	/// <summary>
	// 影描画
	/// </summary>
	/// <param name="rc"></param>
	virtual void CastShadow(const RenderContext& rc);

	/// <summary>
	// 3D描画後の描画処理
	/// </summary>
	/// <param name="rc"></param>
	virtual void DelayedRender(const RenderContext& rc);

	/// <summary>
	// Gui描画
	/// </summary>
	virtual void DrawGui();

	/// <summary>
	/// 当たり判定処理
	/// </summary>
	/// <param name="other">判定対象</param>
	virtual void Judge(Actor* other);

	/// <summary>
	/// 接触時の処理
	/// </summary>
	/// <param name="other">接触対象</param>
	/// <param name="hitPosition">接触位置</param>
	/// <param name="hitNormal">接触対象から自身に向けての法線</param>
	/// <param name="penetration">めり込み量</param>
	virtual void OnCollision(Actor* other, 
		const Vector3& hitPosition, 
		const Vector3& hitNormal, 
		const float& penetration);

	/// <summary>
	// 削除処理
	/// </summary>
	virtual void Destroy();

	/// <summary>
	/// モデルの読み込み
	/// </summary>
	/// <param name="filename"></param>
	/// <returns></returns>
	virtual std::weak_ptr<Model> LoadModel(const char* filename);
#pragma region コンポーネント関係
	/// <summary>
	// コンポーネント追加
	/// </summary>
	template<class T, class... Args>
	std::shared_ptr<T> AddComponent(Args... args)
	{
		std::shared_ptr<T> component = std::make_shared<T>(args...);
		component->SetActor(shared_from_this());
		_components.emplace_back(component);
		return component;
	}

	/// <summary>
	/// コンポーネント取得
	/// </summary>
	/// <typeparam name="T">Componentを継承したもの</typeparam>
	/// <returns>失敗でnullptr</returns>
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

	/// <summary>
	/// 当たり判定コンポーネント追加
	/// </summary>
	/// <typeparam name="T">Colliderを継承したコンポーネント</typeparam>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="...args">引数</param>
	/// <returns></returns>
	template<class T, class... Args>
	std::shared_ptr<T> AddCollider(Args... args)
	{
		std::shared_ptr<T> component = std::make_shared<T>(args...);
		component->SetActor(shared_from_this());
		_colliders.emplace_back(component);
		return component;
	}

	/// <summary>
	/// 当たり判定コンポーネント取得
	/// </summary>
	/// <typeparam name="T">Colliderを継承したコンポーネント</typeparam>
	/// <returns>失敗でnullptr</returns>
	template<class T>
	std::shared_ptr<T> GetCollider()
	{
		for (std::shared_ptr<ColliderBaseComponent>& component : _colliders)
		{
			std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(component);
			if (p == nullptr) continue;
			return p;
		}
		return nullptr;
	}
	std::shared_ptr<ColliderBaseComponent> GetCollider(size_t index)
	{
		return _colliders[index];
	}
	size_t GetColliderComponentSize()const { return _colliders.size(); }
#pragma endregion


#pragma region アクセサ
	Scene* GetScene() { return _scene; }
	const char* GetName() const { return _name.c_str(); }
	Transform& GetTransform() { return _transform; }
	std::weak_ptr<Model> GetModel() { return _model; }
	const std::unordered_map<ActorTag, bool>& GetJudgeTags()const { return _judgeTags; }

	void SetScene(Scene* scene) { this->_scene = scene; }
	void SetName(const char* name) { this->_name = name; }
	void SetTransform(const Transform& t) { this->_transform = t; }
	void SetModel(std::shared_ptr<Model> model) { this->_model = model; }
	// そのタグに対して当たり判定を行うかのフラグをセット
	void SetJudgeTagFlag(ActorTag tag, bool f) { _judgeTags[tag] = f; }

#pragma region フラグ関係
	void SetActiveFlag(bool b) { this->_isActive = b; }
	void SetShowFlag(bool b) { this->_isShowing = b; }
	void SetDrawDebugFlag(bool b) { this->_drawDebug = b; }
	void SetDrawHierarchyFlag(bool b) { this->_drawHierarchy = b; }

	bool IsActive()const { return _isActive; }
	bool IsShowing()const { return _isShowing; }
	bool DrawDebug()const { return _drawDebug; }
	bool DrawHierarchy()const { return _drawHierarchy; }
#pragma endregion
#pragma endregion
protected:
	// トランスフォーム更新
	virtual void UpdateTransform();

	// ギズモ描画
	virtual void DrawGuizmo();

protected:
	// 所属するシーン
	Scene* _scene = nullptr;
	// 名前
	std::string			_name;
	// トランスフォーム
	Transform			_transform;
	// モデル
	std::shared_ptr<Model>	_model;

	bool				_isActive = true;
	bool				_isShowing = true;
	bool				_drawDebug = true;
	bool				_useGuizmo = true;
	bool 				_drawHierarchy = false;

	std::vector<std::shared_ptr<Component>>	_components;
	// 当たり判定コンポーネント
	std::vector<std::shared_ptr<ColliderBaseComponent>>	_colliders;

	// 各タグに対して当たり判定を行うかのフラグ
	std::unordered_map<ActorTag, bool> _judgeTags;
};