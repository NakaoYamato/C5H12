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

	namespace Find
	{
		/// <summary>
		/// 指定要素の取得(updateActorsから検索)
		/// </summary>
		/// <param name="tag">検索するタグ</param>
		/// <returns>そのタグに含まれる全要素</returns>
		ActorMap& ByTag(ActorTag tag);

		/// <summary>
		/// 指定要素の取得(startActorsから検索)
		/// </summary>
		/// <param name="tag">検索するタグ</param>
		/// <returns>そのタグに含まれる全要素</returns>
		ActorMap& ByTagInStartActors(ActorTag tag);

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

	/// <summary>
	/// アクターの登録
	/// </summary>
	/// <typeparam name="T">Actorを継承したクラス</typeparam>
	/// <param name="name"></param>
	/// <param name="tag"></param>
	/// <returns></returns>
	template<class T>
	static std::shared_ptr<T> Register(const std::string& name, ActorTag tag)
	{
#ifdef _DEBUG
		if (Find::ByName(name, tag))
			assert(!"名前の重複");
#endif
		std::shared_ptr<T> actor = std::make_shared<T>();
		actor->SetName(name.c_str());

		// 当たり判定フラグを設定
		for (size_t i = 0; i < static_cast<size_t>(ActorTag::ActorTagMax); ++i)
		{
			actor->SetJudgeTagFlag(static_cast<ActorTag>(i), true);
		}
		actor->SetJudgeTagFlag(ActorTag::DrawContextParameter, false);

		//　startActorsに登録
		Find::ByTagInStartActors(tag).emplace_back(actor);

		return actor;
	}
	
	/// <summary>
	/// アクターの作成
	/// </summary>
	/// <param name="name">オブジェクトの名前（重複不可）</param>
	/// <param name="tag">タグ</param>
	/// <returns>オブジェクトの実体</returns>
	static std::shared_ptr<Actor> Register(const std::string& name, ActorTag tag)
	{
		return Register<Actor>(name, tag);
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