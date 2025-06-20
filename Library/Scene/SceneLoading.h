#pragma once

#include "SceneManager.h"
#include "../../Library/2D/Sprite.h"

#include <thread>
#include <memory>

class SceneLoading : public Scene
{
public:
	SceneLoading(std::shared_ptr<Scene>&& nextScene) :
		_nextScene(std::move(nextScene)) {}
	~SceneLoading()override {}

	// 名前取得
	const char* GetName()const { return u8"Loading"; }
	// 階層取得(アクセスすることがないのでEndを返す)
	SceneMenuLevel GetLevel() const { return SceneMenuLevel::LevelEnd; }

	//初期化
	void Initialize()override;
	//終了化 
	void Finalize()override;
	//更新処理
	void Update(float elapsedTime)override;
	//描画処理
	void Render()override;
	// GUI描画処理
	void DrawGui() override;
	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return nullptr;
	}
	// カメラ取得
	// ロード中はカメラを使用しないためにnullptr
	Camera* GetMainCamera() override {
		return nullptr;
	}
private:
	static void LoadingThread(SceneLoading* scene);

private:
	std::shared_ptr<Scene> _nextScene;
	std::thread* _thread = nullptr;
	float _loadingTimer = 0.0f;

	std::unordered_map<std::string, Sprite> _sprites;
	// ロードバーの幅
	float _loadingBarWidth = 0.0f;
	// ロードバーの進捗速度
	float _loadingBarSpeed = 3.0f;
	// 完了テキストの位置
	Vector2 _completeTextPosition = Vector2(950.0f, 100.0f);
	// 完了テキストの位置
	Vector2 _completeTextScale = Vector2::One;
	// 完了テキストの色
	Vector4 _completeTextColor = Vector4::White;
};



