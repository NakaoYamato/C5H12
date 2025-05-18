#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

#include "../../Library/Math/Transform.h"
#include "../../Library/Graphics/RenderContext.h"
#include "../../Library/Model/Model.h"

// 前方宣言
class Actor;
class Component;
class ColliderBase;
enum class ActorTag;
class Scene;

/// <summary>
/// 接触情報
/// </summary>
struct CollisionData
{
	// 自分のレイヤー
	std::string myLayer = "";
	// 自分がトリガーかどうか
	bool isTrigger = false;
	// 衝突相手
	Actor* other = nullptr;
	// 衝突相手がトリガーかどうか
    bool otherIsTrigger = false;
	// 相手のレイヤー
	std::string otherLayer = "";
	// 衝突点
	Vector3 hitPosition = Vector3::Zero;
	// 衝突点の法線
	Vector3 hitNormal = Vector3::Zero;
	// めり込み量
	float penetration = 0.0f;
};

/// <summary>
/// ゲームオブジェクトの基底クラス
/// </summary>
class Actor : public std::enable_shared_from_this<Actor>
{
public:
	Actor() {}
	virtual ~Actor() {};

#pragma region ActorManagerで呼ぶ関数
	/// <summary>
	/// 生成処理
	/// </summary>
	void Create();
	/// <summary>
	/// 削除時処理
	/// </summary>
	void Deleted();
	/// <summary>
	// 開始処理
	/// </summary>
	void Start();
	/// <summary>
	///	更新処理
	/// </summary>
	/// <param name="elapsedTime"></param>
	void Update(float elapsedTime);
	/// <summary>
	/// Update後更新処理
	/// </summary>
	/// <param name="elapsedTime"></param>
	void LateUpdate(float elapsedTime);
	/// <summary>
	/// 固定間隔更新処理
	/// </summary>
	void FixedUpdate();
	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="rc"></param>
	void Render(const RenderContext& rc);
	/// <summary>
	// デバッグ表示
	/// </summary>
	/// <param name="rc"></param>
	void DebugRender(const RenderContext& rc);
	/// <summary>
	// 影描画
	/// </summary>
	/// <param name="rc"></param>
	void CastShadow(const RenderContext& rc);
	/// <summary>
	// 3D描画後の描画処理
	/// </summary>
	/// <param name="rc"></param>
	void DelayedRender(const RenderContext& rc);
	/// <summary>
	// Gui描画
	/// </summary>
	void DrawGui();
#pragma endregion

	/// <summary>
	/// 接触処理
	/// </summary>
	/// <param name="collisionData">接触情報</param>
	void Contact(CollisionData& collisionData);

	/// <summary>
	// 自身を削除処理
	/// </summary>
	void Remove();

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
		for (std::shared_ptr<ColliderBase>& component : _colliders)
		{
			std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(component);
			if (p == nullptr) continue;
			return p;
		}
		return nullptr;
	}
	std::shared_ptr<ColliderBase> GetCollider(size_t index)
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
	bool IsDrawingDebug()const { return _drawDebug; }
	bool IsDrawingHierarchy()const { return _drawHierarchy; }
#pragma endregion
#pragma endregion
protected:
#pragma region 仮想関数
	/// <summary>
	/// 生成時処理
	/// </summary>
	virtual void OnCreate() {};
	/// <summary>
	/// 削除時処理
	/// </summary>
	virtual void OnDeleted() {};
	/// <summary>
	// 開始時処理
	/// </summary>
	virtual void OnStart() {};
	/// <summary>
	/// 更新前処理
	/// </summary>
	/// <param name="elapsedTime"></param>
	virtual void OnPreUpdate(float elapsedTime) {};
	/// <summary>
	/// 更新時処理
	/// </summary>
	/// <param name="elapsedTime">前フレームからの更新時間</param>
	virtual void OnUpdate(float elapsedTime) {};
	/// <summary> 
	/// Updateのあとによばれる更新時処理
	/// </summary>
	/// <param name="elapsedTime"></param>
	virtual void OnLateUpdate(float elapsedTime) {};
	/// <summary>
	/// 一定間隔の更新時処理
	/// #include "../../Library/Scene/Scene.h"をインクルードして
	/// _FIXED_UPDATE_INTERVAL　が一定間隔(秒)
	/// </summary>
	virtual void OnFixedUpdate() {};
	// トランスフォーム更新
	virtual void UpdateTransform();
	// ギズモ描画
	virtual void DrawGuizmo();
	// GUI描画
	virtual void OnDrawGui() {};

	/// <summary>
	/// 接触時処理
	/// </summary>
	/// <param name="collisionData">接触情報</param>
	virtual void OnContact(CollisionData& collisionData) {}

	/// <summary>
	/// 自身削除時処理
	/// </summary>
	virtual void OnRemove() {};
#pragma endregion

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
	std::vector<std::shared_ptr<ColliderBase>>	_colliders;

	// 各タグに対して当たり判定を行うかのフラグ
	std::unordered_map<ActorTag, bool> _judgeTags;
};