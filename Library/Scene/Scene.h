#pragma once

#include <d3d11.h>
#include <memory>
#include "../../Library/3D/SkyMap.h"

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
	bool IsReady()const { return ready_; }

	// 準備完了設定
	void SetReady() { ready_ = true; }

	// スカイマップ設定
	void SetSkyMap(const wchar_t* filename);
private:
	std::unique_ptr<SkyMap> skyMap_;
	bool ready_ = false;
};