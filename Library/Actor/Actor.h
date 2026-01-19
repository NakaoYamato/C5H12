#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

#include "../../Library/Math/Transform.h"
#include "../../Library/Graphics/RenderContext.h"
#include "../../Library/Model/Model.h"
#include "../../Collision/CollisionDefine.h"

#include "../../Library/DebugSupporter/DebugSupporter.h"
#include "../../Library/ImGui/ImGuiProfiler.h"

// 前方宣言
class Component;
class ColliderBase;
class Scene;

/// <summary>
/// ゲームオブジェクトのタグ
/// </summary>
enum class ActorTag
{
	System,	// 描画の前準備
	Stage,
	Player,
	Enemy,
	UI,

	ActorTagMax
};

// ゲームオブジェクトの基底クラス
class Actor : public std::enable_shared_from_this<Actor>
{
public:
	Actor() {}
	virtual ~Actor() {};

#pragma region ActorManagerで呼ぶ関数
	/// <summary>
	/// 生成処理
	/// </summary>
	void Create(ActorTag tag);
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

#pragma region 当たり判定
	/// <summary>
	/// 接触処理
	/// </summary>
	/// <param name="collisionData">接触情報</param>
	void Contact(CollisionData& collisionData);
	/// <summary>
	/// 接触した瞬間の処理
	/// </summary>
	/// <param name="collisionData">接触情報</param>
	void ContactEnter(CollisionData& collisionData);
	/// <summary>
	///	前フレームに接触したアクター情報をクリア
	/// </summary>
	void CrearLastContactActors()
	{
		_lastContactActors.clear();
	}
	/// <summary>
	/// ファイルからコライダー読み込み
	/// </summary>
	void LoadColliderFromFile();
	/// <summary>
	/// ファイルへコライダー保存
	/// </summary>
	void SaveColliderFromFile();
#pragma endregion

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
		// コンポーネントの生成時処理
		component->OnCreate();
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
	/// コンポーネント一覧取得
	/// </summary>
	/// <returns></returns>
	std::vector<std::shared_ptr<Component>>& GetComponents() { return _components; }

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
		// コンポーネントの生成時処理
		component->OnCreate();
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
	/// <summary>
	/// 当たり判定コンポーネント取得
	/// </summary>
	/// <typeparam name="T">Colliderを継承したすべてのコンポーネント</typeparam>
	/// <returns>失敗でsize() == 0</returns>
	template<class T>
	std::vector<std::shared_ptr<T>> GetColliders()
	{
		std::vector<std::shared_ptr<T>> result;
		for (std::shared_ptr<ColliderBase>& component : _colliders)
		{
			std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(component);
			if (p == nullptr) continue;
			result.push_back(p);
		}
		return result;
	}
#pragma endregion

#pragma region アクセサ
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

	ActorTag				GetTag() const	{ return _tag; }
	Scene*					GetScene()		{ return _scene; }
	const char*				GetName() const { return _name.c_str(); }
	Transform&				GetTransform()	{ return _transform; }
	std::weak_ptr<Model>	GetModel()		{ return _model; }
	std::unordered_map<CollisionLayer, std::vector<Actor*>>& GetLastContactActors() { return _lastContactActors; }

	void SetScene(Scene* scene)					{ this->_scene = scene; }
	void SetName(const char* name)				{ this->_name = name; }
	void SetTransform(const Transform& t)		{ this->_transform = t; }
	void SetModel(std::shared_ptr<Model> model) { this->_model = model; }

	// フォルダパス取得
	const std::string& GetFolderPath() const			{ return _folderPath; }
	// フォルダパス設定
	void SetFolderPath(const std::string& folderPath)	{ _folderPath = folderPath; }
#pragma endregion

#pragma region フラグ関係
	void SetIsActive(bool b);
	void SetIsDrawingTransformGui(bool b)	{ this->_isDrawingTransformGui = b; }
	void SetIsShowing(bool b)				{ this->_isShowing = b; }
	void SetIsCastingShadow(bool b)			{ this->_isCastingShadow = b; }
	void SetIsDrawingDebug(bool b)			{ this->_isDrawingDebug = b; }
	void SetIsUsingGuizmo(bool b)			{ this->_isUsingGuizmo = b; }
	void SetIsDrawingHierarchy(bool b)		{ this->_isDrawingHierarchy = b; }
	void SetIsDrawingInspector(bool b)		{ this->_isDrawingInspector = b; }
	void SetInheritParentTransform(bool b)	{ this->_isInheritParentTransform = b; }

	bool IsActive()const					{ return _isActive; }
	bool IsDrawingTransformGui()const		{ return _isDrawingTransformGui; }
	bool IsShowing()const					{ return _isShowing; }
	bool IsCastingShadow()const				{ return _isCastingShadow; }
	bool IsDrawingDebug()const				{ return _isDrawingDebug; }
	bool IsUsingGuizmo()const				{ return _isUsingGuizmo; }
	bool IsDrawingHierarchy()const			{ return _isDrawingHierarchy; }
	bool IsDrawingInspector()const			{ return _isDrawingInspector; }
	bool IsInheritParentTransform()const	{ return _isInheritParentTransform; }
#pragma endregion

#pragma region 親子関係
	/// <summary>
	/// 親設定
	/// </summary>
	/// <param name="parent"></param>
	virtual void SetParent(Actor* parent);
	/// <summary>
	/// 子追加
	/// </summary>
	/// <param name="child"></param>
	virtual void AddChild(std::shared_ptr<Actor> child);	
	/// <summary>
	/// 親関係解除
	/// </summary>
	virtual void ReleaseParent();
	/// <summary>
	/// 親取得
	/// </summary>
	/// <returns></returns>
	Actor* GetParent() { return _parent; }
	/// <summary>
	/// 子供取得
	/// </summary>
	/// <returns></returns>
	std::vector<std::shared_ptr<Actor>>& GetChildren() { return _children; }
	/// <summary>
	/// 親の名前取得　親がなければ空文字
	/// </summary>
	/// <returns></returns>
	virtual std::string GetParentName()
	{
		return _parent ? _parent->GetName() : "";
	}
#pragma endregion

#pragma region 入出力
	// ファイルパス取得
	virtual std::string GetFilePath();

	// ファイル読み込み
	virtual bool LoadFromFile() { return false; }
	// ファイル保存
	virtual bool SaveToFile() { return false; }
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
	/// 開始時処理
	/// </summary>
	virtual void OnStart() {};
	/// <summary>
	/// 更新前処理
	/// </summary>
	/// <param name="elapsedTime"></param>
	virtual void OnPreUpdate(float elapsedTime) {};
	/// <summary>
	/// モデルのトランスフォーム更新
	/// </summary>
	virtual void UpdateModelTransform();
	/// <summary>
	/// 更新時処理
	/// </summary>
	/// <param name="elapsedTime">前フレームからの更新時間</param>
	virtual void OnUpdate(float elapsedTime) {};
	/// <summary>
	/// トランスフォーム更新
	/// </summary>
	virtual void UpdateTransform();
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
	/// <summary>
	/// 描画時処理
	/// </summary>
	/// <param name="rc"></param>
	virtual void OnRender(const RenderContext& rc) {};
	/// <summary>
	/// デバッグ表示時処理
	/// </summary>
	/// <param name="rc"></param>
	virtual void OnDebugRender(const RenderContext& rc) {};
	/// <summary>
	/// 3D描画後の描画時処理
	/// </summary>
	/// <param name="rc"></param>
	virtual void OnDelayedRender(const RenderContext& rc) {};
	/// <summary>
	/// トランスフォームGUI描画
	/// </summary>
	virtual void DrawTransformGui();
	/// <summary>
	/// ギズモ描画
	/// </summary>
	virtual void DrawGuizmo();
	/// <summary>
	/// GUI描画時処理
	/// </summary>
	virtual void OnDrawGui() {};
	/// <summary>
	/// コンポーネントGUI描画
	/// </summary>
	virtual void DrawComponentGui();
	/// <summary>
	/// コライダーGUI描画
	/// </summary>
	virtual void DrawColliderGui();
	/// <summary>
	/// 接触時処理
	/// </summary>
	/// <param name="collisionData">接触情報</param>
	virtual void OnContact(CollisionData& collisionData) {}
	/// <summary>
	/// 接触した瞬間の処理
	/// </summary>
	/// <param name="collisionData">接触情報</param>
	virtual void OnContactEnter(CollisionData& collisionData) {}
	/// <summary>
	/// 起動フラグが変化したときの処理
	/// </summary>
	/// <param name="isActive">変化後の値</param>
	virtual void ChangedActive(bool isActive);
	/// <summary>
	/// 起動フラグが変化したときの処理
	/// </summary>
	/// <param name="isActive">変化後の値</param>
	virtual void OnChangedActive(bool isActive) {}
#pragma endregion

protected:
	// タグ
	ActorTag				_tag = ActorTag::ActorTagMax; // 初期値は無効なタグ
	// 所属するシーン
	Scene*					_scene = nullptr;
	// 名前
	std::string				_name;
	// トランスフォーム
	Transform				_transform;
	// モデル
	std::shared_ptr<Model>	_model;
	// コンポーネント
	std::vector<std::shared_ptr<Component>>		_components;
	// フォルダパス
	std::string 			_folderPath;

#pragma region 親子関係
	Actor*								_parent = nullptr;
	std::vector<std::shared_ptr<Actor>> _children;
#pragma endregion

#pragma region 当たり判定
	// 当たり判定コンポーネント
	std::vector<std::shared_ptr<ColliderBase>>	_colliders;
	// 自身のレイヤーごとの前フレームに接触したレイヤーごとのアクター
	std::unordered_map<CollisionLayer, std::vector<Actor*>> _lastContactActors;
#pragma endregion

#pragma region 各種フラグ
	// アクティブフラグ
	bool				_isActive = true;
	// トランスフォームGUI描画フラグ
	bool				_isDrawingTransformGui = true;
	// 表示フラグ
	bool				_isShowing = true;
	// 影描画フラグ
	bool				_isCastingShadow = true;
	// デバッグ表示フラグ
	bool				_isDrawingDebug = true;
	// ギズモ使用フラグ
	bool				_isUsingGuizmo = true;
	// ヒエラルキー描画フラグ
	bool 				_isDrawingHierarchy = true;
	// インスペクター描画フラグ
	bool				_isDrawingInspector = false;
	// 親のトランスフォームを反映するか
	bool				_isInheritParentTransform = true;
	// プロファイラーを開くか
	bool				_isOpenProfiler = false;
#pragma endregion

#pragma region デバッグ用
	static Vector4 OpenHeaderColor;
	static Vector4 OpenHeaderHoveredColor;
	static Vector4 OpenHeaderActiveColor;

	// プロファイラー
	ImGuiControl::Profiler _profiler;
#pragma endregion
};