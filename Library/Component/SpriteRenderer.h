#pragma once

#include "Component.h"
#include "../../Library/2D/Sprite.h"

#include <unordered_map>

/// <summary>
/// 画像描画コンポーネント
/// </summary>
class SpriteRenderer : public Component
{
public:
	class SpriteData : public Sprite
	{
	public:
		std::string _parentName = "";
	};

public:
	SpriteRenderer() {}
	~SpriteRenderer() override = default;
	// 名前取得
	const char* GetName() const override { return "SpriteRenderer"; }

	// 生成時処理
	void OnCreate() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 3D描画後の描画処理
	void DelayedRender(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;
	// 画像読み込み
	void LoadTexture(const std::string& spriteName, const wchar_t* filename, Sprite::CenterAlignment alignment = Sprite::CenterAlignment::CenterCenter);
	// 画像との当たり判定
	bool IsHit(const std::string& name, const Vector2& pos) const;

#pragma region アクセサ
	Sprite::CenterAlignment GetCenterAlignment(const std::string& name) const { return _sprites.at(name).GetCenterAlignment(); }
	RectTransform& GetRectTransform(const std::string& name) { return _sprites.at(name).GetRectTransform(); }
	const Vector2& GetTexPos(const std::string& name)const { return _sprites.at(name).GetTexPos(); }
	const Vector2& GetTexSize(const std::string& name)const { return _sprites.at(name).GetTexSize(); }
	const Vector2& GetCenter(const std::string& name)const { return _sprites.at(name).GetCenter(); }
	const Vector4& GetColor(const std::string& name)const { return _sprites.at(name).GetColor(); }
    DepthState GetDepthState(const std::string& name) const { return _sprites.at(name).GetDepthState(); }
    int GetStencil(const std::string& name) const { return _sprites.at(name).GetStencil(); }
	const std::string& GetParentName(const std::string& name) const { return _sprites.at(name)._parentName; }
	float GetOverallAlpha() const { return _overallAlpha; }
	bool IsLoaded() const { return _isLoaded; }

	void SetCenterAlignment(const std::string& name, Sprite::CenterAlignment alignment) {
		_sprites.at(name).SetCenterAlignment(alignment);
		_sprites.at(name).RecalcCenter(alignment);
	}
	void SetTexPos(const std::string& name, const Vector2& p) { _sprites.at(name).SetTexPos(p); }
	void SetTexSize(const std::string& name, const Vector2& s) { _sprites.at(name).SetTexSize(s); }
	void SetCenter(const std::string& name, const Vector2& c) { _sprites.at(name).SetCenter(c); }
	void SetColor(const std::string& name, const Vector4& c) { _sprites.at(name).SetColor(c); }
	void SetColorAlpha(const std::string& name, float a) { _sprites.at(name).SetColorAlpha(a); }
    void SetDepthState(const std::string& name, DepthState ds) { _sprites.at(name).SetDepthState(ds); }
    void SetStencil(const std::string& name, int stencil) { _sprites.at(name).SetStencil(stencil); }
	void SetParentName(const std::string& name, const std::string& parentName) { _sprites.at(name)._parentName = parentName; }
	void SetOverallAlpha(float a) { _overallAlpha = a; }
#pragma endregion

#pragma region 入出力
	// ファイル読み込み
	bool LoadFromFile() override;
	// ファイル保存
	bool SaveToFile() override;
#pragma endregion

protected:
	// スプライト描画
	virtual void SpriteRender(
		const std::string& spriteName,
		const RenderContext& rc);

private:
	RectTransform* _myRectTransform = nullptr;

	std::unordered_map<std::string, SpriteData> _sprites;
    std::vector<std::string> _spriteDrawOrder;

	// 全体透明度
	float _overallAlpha = 1.0f;
	// 読みこみが成功したか
	bool _isLoaded = false;
};