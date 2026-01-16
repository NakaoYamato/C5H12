#pragma once

#include <d3d11.h>
#include <memory>

#include "../../Library/Graphics/RenderContext.h"
#include "../../Library/JobSystem/JobSystem.h"
#include "../../Library/2D/SkyMap.h"
#include "../../Library/2D/Primitive.h"
#include "../../Library/2D/Fade.h"
#include "../../Library/2D/InputUI.h"
#include "../../Library/Renderer/MeshRenderer.h"
#include "../../Library/Renderer/TextureRenderer.h"
#include "../../Library/Renderer/TextRenderer.h"
#include "../../Library/Renderer/TerrainRenderer.h"
#include "../../Library/Renderer/ParticleRenderer.h"
#include "../../Library/Renderer/PrimitiveRenderer.h"
#include "../../Library/Renderer/DecalRenderer.h"

#include "../../Library/Actor/ActorManager.h"
#include "../../Library/Collision/CollisionManager.h"
#include "../../Library/Effekseer/EffekseerEffectManager.h"

#include "../../Library/Actor/Camera/MainCamera.h"
#include "../../Library/Component/Light/LightController.h"

#pragma region 定義
#define _RENDER_FRAME_INDEX         0
#define _APPLY_SHADOW_FRAME_INDEX   1
#define _SCENE_FRAME_INDEX			2

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

#pragma region SceneManagerで呼ぶ関数
	// 初期化
	virtual void Initialize();
	// 終了化
	virtual void Finalize();
	// 更新処理
	virtual void Update(float elapsedTime);
	// 一定間隔の更新処理
	virtual void FixedUpdate();
	// 描画処理
	virtual void Render();
	// GUI描画処理
	virtual void DrawGui();
#pragma endregion

#pragma region 仮想関数
	// 初期化
	virtual void OnInitialize() {}
	// 終了化
	virtual void OnFinalize() {}
	// 更新処理
	virtual void OnUpdate(float elapsedTime) {}
	// 一定間隔の更新処理
	virtual void OnFixedUpdate() {}
	// 描画処理
	virtual void OnRender() {}
	// GUI描画処理
	virtual void OnDrawGui() {}
#pragma endregion

#pragma region アクター
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
		actor->Create(tag);
		_actorManager.Register(actor, tag);
		return actor;
	}
#pragma endregion

#pragma region 各種レンダラー取得	
	// メッシュレンダラー取得
	MeshRenderer& GetMeshRenderer();
	// テクスチャレンダラー取得
	TextureRenderer& GetTextureRenderer();
	// テキストレンダラー取得
	TextRenderer& GetTextRenderer();
	// テレインレンダラー取得
	TerrainRenderer& GetTerrainRenderer();
	// パーティクルレンダラー取得
	ParticleRenderer& GetParticleRenderer();
	// プリミティブレンダラー取得	
	PrimitiveRenderer& GetPrimitiveRenderer();
	// デカールレンダラー取得
	DecalRenderer& GetDecalRenderer();
#pragma endregion

#pragma region アクセサ
	// 自身を新規のポインタで渡す
	virtual std::shared_ptr<Scene> GetNewShared() = 0;

	// アクター管理者取得
	ActorManager&			GetActorManager()			{ return _actorManager; }
	// 当たり判定管理者取得
	CollisionManager&		GetCollisionManager()		{ return _collisionManager; }
	// RenderContext取得
	RenderContext&			GetRenderContext()			{ return _renderContext; }
    // プリミティブ取得
	Primitive* GetPrimitive();
	// Effekseerエフェクトマネージャー取得
	EffekseerEffectManager& GetEffekseerEffectManager();

	// 平行光源取得
	std::shared_ptr<LightController> GetDirectionalLight() {
		return _directionalLight.lock();
	}
	// フェード情報取得
	Fade* GetFade();
	// 入力UI取得
	InputUI* GetInputUI();
	// カメラ取得
	virtual Camera* GetMainCamera() {
		return &_camera;
	}
	// カメラアクター取得
	virtual std::shared_ptr<MainCamera> GetMainCameraActor() {
		return _mainCameraActor.lock();
	}

	// スカイマップ設定
	void SetSkyMap(const wchar_t* filename, const wchar_t* diffuseIEM, const wchar_t* specularIDM);

	// 経過時間取得
	float GetTime() const { return _time; }

	// 画面サイズ取得
	float GetScreenWidth() const;
	// 画面サイズ取得
	float GetScreenHeight() const;

	// グリッド表示フラグ取得
	void SetShowGrid(bool show) { _showGrid = show; }
	// 準備完了しているか
	bool IsReady()const { return _isReady; }
	// ロード進捗率取得
	float GetCompletionLoading() const
	{
		return _completionLoading;
	}
	// ロード進捗率設定
	void SetCompletionLoading(float completion)
	{
		_completionLoading = std::clamp(completion, 0.0f, 1.0f);
	}
	// ロード進捗率設定
	void AddCompletionLoading(float completion)
	{
		_completionLoading = std::clamp(_completionLoading + completion, 0.0f, 1.0f);
	}
	// ImGuiに描画中か
	bool IsImGuiRendering() const { return _isImGuiRendering; }
	// ImGuiウィンドウが選択されているか
	bool IsImGuiSceneWindowSelected() const { return _isImGuiSceneWindowSelected; }
#pragma endregion
private:
#pragma region シーン必須オブジェクト
	std::weak_ptr<LightController>	_directionalLight;
	std::weak_ptr<MainCamera>		_mainCameraActor;
#pragma endregion

	std::unique_ptr<SkyMap>			_skyMap;
	Camera							_camera = {};
	RenderContext					_renderContext;

	ActorManager					_actorManager;
	CollisionManager				_collisionManager;
	EffekseerEffectManager			_effekseerEffectManager;

	// 経過時間
	float							_time = 0.0f;
	
	// 準備完了フラグ
	bool							_isReady = false;
	// ロード進捗率
	float							_completionLoading = 0.0f;
	// グリッド表示フラグ
	bool							_showGrid = false;
	// ImGuiに描画中
	bool							_isImGuiRendering = false;
	// シーンを描画しているImGuiウィンドウを選択しているかどうか
	bool							_isImGuiSceneWindowSelected = false;
};