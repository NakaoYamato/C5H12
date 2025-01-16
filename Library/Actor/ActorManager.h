#pragma once

#include <unordered_map>
#include <vector>
#include <set>
#include <string>
#include <memory>

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

namespace ActorManager
{
	// 更新処理
	void Update(float elapsedTime);

	// 1秒ごとの更新処理
	void FixedUpdate();

	// 当たり判定処理
	void Judge();

	// 描画の前処理
	void RenderPreprocess(RenderContext& rc);

	// 描画処理
	void Render(const RenderContext& rc);

	// 影描画処理
	void CastShadow(const RenderContext& rc);

	// 3D描画後の描画処理
	void DelayedRender(RenderContext& rc);

	// Gui描画
	void DrawGui();

	/// <summary>
	/// アクターの作成
	/// </summary>
	/// <param name="name">オブジェクトの名前（重複不可）</param>
	/// <param name="tag">タグ</param>
	/// <returns>オブジェクトの実体</returns>
	std::shared_ptr<Actor> Create(const std::string& name, ActorTag tag);

	namespace Find
	{
		/// <summary>
		/// 指定要素の取得
		/// </summary>
		/// <param name="tag">検索するタグ</param>
		/// <returns>そのタグに含まれる全要素</returns>
		ActorMap& ByTag(ActorTag tag);
		/// <summary>
		/// 指定要素の取得
		/// </summary>
		/// <param name="name">検索する名前</param>
		/// <param name="tag">検索する際の補助、GameObjectTagMax指定で全タグから検索</param>
		/// <returns>成功でポインタ、失敗でnullptr</returns>
		std::shared_ptr<Actor> ByName(const std::string& name, ActorTag tag = ActorTag::ActorTagMax);

		/// <summary>
		/// 指定要素を開始アクターから取得
		/// </summary>
		/// <param name="name">検索する名前</param>
		/// <param name="tag">検索する際の補助、GameObjectTagMax指定で全タグから検索</param>
		/// <returns>成功でポインタ、失敗でnullptr</returns>
		std::shared_ptr<Actor> ByNameFromStartActor(const std::string& name, ActorTag tag = ActorTag::ActorTagMax);

	}

	// 要素の全削除
	void Clear();

	// 指定要素の削除
	void Remove(std::shared_ptr<Actor> actor);
	void Remove(const std::string& name);
	void Remove(ActorTag tag);

	// ゲームスピードの設定
	void SetGameSpeed(ActorTag tag, float scale, float duration);
}