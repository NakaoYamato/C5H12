#pragma once

#include <unordered_map>
#include <vector>
#include <set>
#include <string>
#include <memory>
#include <mutex>

#include "Actor.h"

/// <summary>
/// ゲームオブジェクトのタグ
/// </summary>
enum class ActorTag
{
	DrawContextParameter,	// 描画の前準備
	Stage,
	Player,
	Enemy,

	ActorTagMax
};

using ActorMap = std::vector<std::shared_ptr<Actor>>;

class ActorManager
{
public:
	ActorManager() {}
	~ActorManager() {}

	// 更新処理
	void Update(float elapsedTime);

	/// 一定間隔の更新処理
	void FixedUpdate();

	// 描画処理
	void Render(const RenderContext& rc);

	// 影描画処理
	void CastShadow(const RenderContext& rc);

	// 3D描画後の描画処理
	void DelayedRender(RenderContext& rc);

	// Gui描画
	void DrawGui();

#pragma region 要素の取得
	/// <summary>
	/// 指定要素の取得(updateActorsから検索)
	/// </summary>
	/// <param name="tag">検索するタグ</param>
	/// <returns>そのタグに含まれる全要素</returns>
	ActorMap& FindByTag(ActorTag tag);

	/// <summary>
	/// 指定要素の取得(startActorsから検索)
	/// </summary>
	/// <param name="tag">検索するタグ</param>
	/// <returns>そのタグに含まれる全要素</returns>
	ActorMap& FindByTagInStartActors(ActorTag tag);

	/// <summary>
	/// 指定要素の取得
	/// </summary>
	/// <param name="name">検索する名前</param>
	/// <param name="tag">検索する際の補助、GameObjectTagMax指定で全タグから検索</param>
	/// <returns>成功でポインタ、失敗でnullptr</returns>
	std::shared_ptr<Actor> FindByName(const std::string& name, ActorTag tag = ActorTag::ActorTagMax);

	/// <summary>
	/// 指定要素を開始アクターから取得
	/// </summary>
	/// <param name="name">検索する名前</param>
	/// <param name="tag">検索する際の補助、GameObjectTagMax指定で全タグから検索</param>
	/// <returns>成功でポインタ、失敗でnullptr</returns>
	std::shared_ptr<Actor> FindByNameFromStartActor(const std::string& name, ActorTag tag = ActorTag::ActorTagMax);
#pragma endregion

	/// <summary>
	/// アクター登録
	/// </summary>
	/// <param name="actor"></param>
	/// <param name="tag"></param>
	void Register(std::shared_ptr<Actor> actor, ActorTag tag);

#pragma region 要素の削除
	/// <summary>
	/// ポインタから要素を削除
	/// </summary>
	/// <param name="actor">要素</param>
	void Remove(std::shared_ptr<Actor> actor);

	/// <summary>
	/// 名前から要素を削除
	/// </summary>
	/// <param name="name">要素の名前</param>
	void Remove(const std::string& name);

	/// <summary>
	/// タグに含まれる要素を全削除
	/// </summary>
	/// <param name="tag">タグ</param>
	void Remove(ActorTag tag);

	// 要素の全削除
	void Clear();
#pragma endregion

	// ゲームスピードの設定
	void SetGameSpeed(ActorTag tag, float scale, float duration);

private:
	ActorMap _startActors[static_cast<size_t>(ActorTag::ActorTagMax)];
	ActorMap _updateActors[static_cast<size_t>(ActorTag::ActorTagMax)];
	std::set<std::shared_ptr<Actor>> _selectionActors;
	std::set<std::shared_ptr<Actor>> _removeActors;

	// 複数スレッドでのアクセスを防ぐためのミューテックス
	std::mutex _mutex;

	// GUIで選択しているオブジェクト
	std::string _showGuiObj = "";

	std::pair<float, float> _gameSpeeds[static_cast<size_t>(ActorTag::ActorTagMax)];
};