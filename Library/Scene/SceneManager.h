#pragma once

#include <memory>
#include <unordered_map>
#include <string>

#include "Scene.h"

// シーンマネージャーのGUIに登録するための構造体
template<class T>
struct SceneRegister
{
	SceneRegister();
};

#pragma region マクロ
// シーンマネージャーのGUIに登録するためのマクロ
// type			: Scene継承クラス
#define _REGISTER_SCENE(type) \
static SceneRegister<type> type##register;
#pragma endregion

//---------------------------------------------------
//  シーン管理クラス
//---------------------------------------------------
class SceneManager
{
private:
	SceneManager() {}
	~SceneManager() {}

public:
	// インスタンス取得
	static SceneManager& Instance() {
		static SceneManager ins_;
		return ins_;
	}

	// 初期化
	void Initialize();

    // 終了化
    void Finalize();

	// 更新処理
	void Update(float elapsedTime);

	// 1秒ごとの更新処理
	void FixedUpdate();

	// 描画処理
	void Render();

	// Gui描画
	void DrawGui();

	// GUIのタブからシーン変更
	void SceneMenuGui();

	// シーンクリア
	void Clear();

	// シーン切り替え
	// REGISTER_SCENE_MANAGERで登録したときの名前から切り替え
	void ChangeScene(SceneMenuLevel level, std::string sceneName);

	// シーン切り替え
	void ChangeScene(std::shared_ptr<Scene> scene);

	// デバッグ用シーンに登録
	template<class T>
	void PushScene()
	{
		auto scene = std::make_unique<T>();
		_sceneDatas[static_cast<int>(scene->GetLevel())][scene->GetName()] = std::make_unique<T>();
	}
	
	std::shared_ptr<Scene> GetCurrentScene()
	{
		return _currentScene;
	}

	Primitive* GetPrimitive() const { return _primitive.get(); }

	// フェード情報取得
	Fade* GetFade() {
		return &_fade;
	}
	// 入力UI取得
	InputUI* GetInputUI() {
		return &_inputUI;
	}
#pragma region 各種レンダラー取得
	// メッシュレンダラー取得
	MeshRenderer& GetMeshRenderer() { return _meshRenderer; }
	// テクスチャレンダラー取得
	TextureRenderer& GetTextureRenderer() { return _textureRenderer; }
	// テキストレンダラー取得
	TextRenderer& GetTextRenderer() { return _textRenderer; }
	// テレインレンダラー取得
	TerrainRenderer& GetTerrainRenderer() { return _terrainRenderer; }
	// パーティクルレンダラー取得
	ParticleRenderer& GetParticleRenderer() { return _particleRenderer; }
	// プリミティブレンダラー取得	
	PrimitiveRenderer& GetPrimitiveRenderer() { return _primitiveRenderer; }
	// デカールレンダラー取得
	DecalRenderer& GetDecalRenderer() { return _decalRenderer; }
#pragma endregion
private:
	std::shared_ptr<Scene> _currentScene;
	std::shared_ptr<Scene> _nextScene;

	// 保存しているシーン
	std::unordered_map<std::string, std::unique_ptr<Scene>> _sceneDatas[static_cast<int>(SceneMenuLevel::LevelEnd)];

	// Debugビルドでのデフォルトシーン
	int _defaultSceneLevel = static_cast<int>(SceneMenuLevel::Debug);
	std::string _defaultSceneName = "";

	std::unique_ptr<Primitive>		_primitive;

#pragma region 各種レンダラー
	MeshRenderer					_meshRenderer;
	TextureRenderer					_textureRenderer;
	TextRenderer					_textRenderer;
	TerrainRenderer					_terrainRenderer;
	ParticleRenderer				_particleRenderer;
	PrimitiveRenderer				_primitiveRenderer;
	DecalRenderer					_decalRenderer;
#pragma endregion

	Fade 							_fade;
	InputUI							_inputUI;
};

template<class T>
inline SceneRegister<T>::SceneRegister()
{
	SceneManager::Instance().PushScene<T>();
}
