#pragma once

#include <d3d11.h>
#include <memory>

#include "../../Library/Graphics/RenderContext.h"
#include "../../Library/2D/SkyMap.h"
#include "../../Library/Renderer/MeshRenderer.h"
#include "../../Library/Renderer/TextureRenderer.h"
#include "../../Library/Renderer/TextRenderer.h"

#include "../../Library/Actor/ActorManager.h"
#include "../../Library/Collision/CollisionManager.h"
#include "../../Library/Effekseer/EffekseerEffectManager.h"

#include "../../Library/Component/Light/LightController.h"
#include "../../Library/Camera/Camera.h"

#pragma region 定義
#define _RENDER_FRAME_INDEX         0
#define _APPLY_SHADOW_FRAME_INDEX   1

#define _SKYMAP_COLOR_SRV_SLOT_INDEX	10 
#define _SKYMAP_DIFFUSE_SRV_SLOT_INDEX	11 
#define _SKYMAP_SPECULAR_SRV_SLOT_INDEX 12 
#define _SKYMAP_LUT_SRV_SLOT_INDEX		13 

#define _SCENE_CB_SLOT_INDEX 0
#define _LIGHT_CB_SLOT_INDEX 3

static constexpr float _FIXED_UPDATE_RATE = 1.0f / 60.0f;
#pragma endregion

// シーンをメニューバーに登録する際の階層
enum class SceneMenuLevel
{
	Editor,
	Debug,
	Game,

	LevelEnd
};

// シーン基底クラス
class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	// 名前取得
	virtual const char* GetName() const = 0;
	// 階層取得
	virtual SceneMenuLevel GetLevel() const = 0;

	// 初期化
	virtual void Initialize();

	// 終了化
	virtual void Finalize();

	// 更新処理
	virtual void Update(float elapsedTime);

	/// 一定間隔の更新処理
	virtual void FixedUpdate();

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
	template<class T, class... Args>
	std::shared_ptr<T> RegisterActor(const std::string& name, ActorTag tag, Args... args)
	{
		std::shared_ptr<T> actor = std::make_shared<T>(args...);
		actor->SetName(name.c_str());
		actor->SetScene(this);
		_actorManager.Register(actor, tag);
		actor->Create();
		return actor;
	}

#pragma region アクセサ
	// アクター管理者取得
	ActorManager& GetActorManager() { return _actorManager; }
	// 当たり判定管理者取得
	CollisionManager& GetCollisionManager() { return _collisionManager; }
	// Effekseerエフェクトマネージャー取得
	EffekseerEffectManager* GetEffekseerEffectManager() { return _effekseerEffectManager.get(); }
	// RenderContext取得
	RenderContext& GetRenderContext() {	return _renderContext; }
	// メッシュレンダラー取得
	MeshRenderer& GetMeshRenderer() { return _meshRenderer; }
	// テクスチャレンダラー取得
	TextureRenderer& GetTextureRenderer() { return _textureRenderer; }
	// テキストレンダラー取得
	TextRenderer& GetTextRenderer() { return _textRenderer; }

	// 平行光源取得
	std::shared_ptr<LightController> GetDirectionalLight() {
		return _directionalLight.lock();
	}
	// カメラ取得
	virtual Camera* GetMainCamera() {
		return &_camera;
	}
#pragma endregion
private:
	std::unique_ptr<SkyMap>			_skyMap;

#pragma region シーン必須オブジェクト
	std::weak_ptr<LightController>	_directionalLight;
#pragma endregion

	Camera							_camera = {};
	RenderContext					_renderContext;
	MeshRenderer					_meshRenderer;
	TextureRenderer					_textureRenderer;
	TextRenderer					_textRenderer;

	ActorManager					_actorManager;
	CollisionManager				_collisionManager;
	std::unique_ptr<EffekseerEffectManager>			_effekseerEffectManager;

	bool							_ready = false;
};