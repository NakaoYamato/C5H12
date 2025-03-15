#pragma once

#include <d3d11.h>
#include <memory>

#include "../../Library/3D/SkyMap.h"
#include "../../Library/Actor/ActorManager.h"

//---------------------------------------------------
//  シーン基底クラス
//---------------------------------------------------
class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	// 名前取得
	virtual const char* GetName() const = 0;

	// 初期化
	virtual void Initialize() = 0;

	// 終了化
	virtual void Finalize();

	// 更新処理
	virtual void Update(float elapsedTime);

	// 1秒ごとの更新処理
	virtual void FixedUpdate() {};

	// 描画処理
	virtual void Render();

	// Gui描画処理
	virtual void DrawGui();

	// 自身を新規のポインタで渡す
	virtual std::shared_ptr<Scene> GetNewShared() = 0;

	// 準備完了しているか
	bool IsReady()const { return _ready; }

	// 準備完了設定
	void SetReady() { _ready = true; }

	// スカイマップ設定
	void SetSkyMap(const wchar_t* filename, const wchar_t* diffuseIEM, const wchar_t* specularIDM);

	/// <summary>
	/// アクター登録
	/// </summary>
	/// <typeparam name="T">アクターを継承したクラス</typeparam>
	/// <param name="name">オブジェクトの名前（重複不可）</param>
	/// <param name="tag">タグ</param>
	/// <returns></returns>
	template<class T>
	std::shared_ptr<T> RegisterActor(const std::string& name, ActorTag tag)
	{
		std::shared_ptr<T> actor = std::make_shared<T>();
		actor->SetName(name.c_str());
		actor->SetScene(this);
		_actorManager.Register(actor, tag);
		return actor;
	}

	// アクター管理者取得
	ActorManager& GetActorManager() { return _actorManager; }

private:
	std::unique_ptr<SkyMap> _skyMap;
	ActorManager _actorManager;
	bool _ready = false;
};