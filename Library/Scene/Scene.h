#pragma once

#include <d3d11.h>
#include <memory>

#include "../../Library/2D/Sprite.h"
#include "../../Library/3D/SkyMap.h"
#include "../../Library/Actor/ActorManager.h"

#include "../../Library/Graphics/RenderContext.h"
#include "../../Library/Component/Light/LightController.h"

#pragma region 定義
#define _RENDER_FRAME_INDEX         0
#define _APPLY_SHADOW_FRAME_INDEX   1

#define _SKYMAP_COLOR_SRV_SLOT_INDEX	10 
#define _SKYMAP_DIFFUSE_SRV_SLOT_INDEX	11 
#define _SKYMAP_SPECULAR_SRV_SLOT_INDEX 12 
#define _SKYMAP_LUT_SRV_SLOT_INDEX		13 

#define _SCENE_CB_SLOT_INDEX 0
#define _LIGHT_CB_SLOT_INDEX 3
#pragma endregion

// シーン基底クラス
class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	// 名前取得
	virtual const char* GetName() const = 0;

	// 初期化
	virtual void Initialize();

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
		actor->OnCreate();
		return actor;
	}

	// アクター管理者取得
	ActorManager& GetActorManager() { return _actorManager; }

	// RenderContext取得
	RenderContext& GetRenderContext() { return _renderContext; }

private:
	std::unique_ptr<Sprite> _fullscreenQuad;
	std::unique_ptr<SkyMap> _skyMap;

	std::weak_ptr<LightController> _directionalLight;

	RenderContext _renderContext;
	ActorManager _actorManager;
	bool _ready = false;
};