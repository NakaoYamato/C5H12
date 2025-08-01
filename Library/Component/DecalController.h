#pragma once

#include "Component.h"
#include "../../Library/Decal/Decal.h"

class DecalController : public Component
{
public:
	DecalController(const std::wstring& colorTextureFilename, const std::wstring& normalTextureFilename) :
		_colorTextureFilename(colorTextureFilename),
		_normalTextureFilename(normalTextureFilename)
	{}
	~DecalController() override = default;
	// 名前取得
	const char* GetName() const override { return "DecalController"; }
	// 開始処理
	void Start() override;
	// 描画処理
	void Render(const RenderContext& rc) override;
	// デバッグ表示
	void DebugRender(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;

private:
	std::wstring _colorTextureFilename;
	std::wstring _normalTextureFilename;
	std::unique_ptr<Decal> _decal;
	Vector4 _color = Vector4::White; // デカールの色
};