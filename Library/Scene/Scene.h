#pragma once

#include <d3d11.h>
#include <memory>

//---------------------------------------------------
//  シーン基底クラス
//---------------------------------------------------
class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	// 初期化
	virtual void Initialize() = 0;

	// 終了化
	virtual void Finalize() = 0;

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 1秒ごとの更新処理
	virtual void FixedUpdate() {};

	// 描画処理
	virtual void Render() = 0;

	// Gui描画処理
	virtual void DrawGui() {}

	// 自身を新規のポインタで渡す
	virtual std::shared_ptr<Scene> GetNewShared() = 0;

	// 準備完了しているか
	bool IsReady()const { return ready_; }

	// 準備完了設定
	void SetReady() { ready_ = true; }

private:
	bool ready_ = false;
};