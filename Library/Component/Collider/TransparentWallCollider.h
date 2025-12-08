#pragma once

#include "MeshCollider.h"
#include "../../Library/Stage/TransparentWall.h"

class TransparentWallCollider : public MeshCollider
{
public:
	TransparentWallCollider(std::string filepath);
	~TransparentWallCollider() override {}
	// 名前取得
	const char* GetName() const override { return "MeshCollider"; }
	// 更新処理
	void Update(float elapsedTime) override;
	// デバッグ描画処理
	void DebugRender(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;
	// コリジョンメッシュの再計算
	CollisionMesh RecalculateCollisionMesh(Model* model) const override;

	// 透明壁取得
	TransparentWall& GetTransparentWall() { return _transparentWall; }
private:
	// 透明壁
	TransparentWall _transparentWall;
	// ファイルパス
	std::string _filepath;
};