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
	SpriteRenderer() {}
	~SpriteRenderer() override = default;
	// 名前取得
	const char* GetName() const override { return "SpriteRenderer"; }
	// 3D描画後の描画処理
	void DelayedRender(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;
	// 画像読み込み
	void LoadTexture(const std::string& spriteName, const wchar_t* filename, Sprite::CenterAlignment alignment);
	// 画像との当たり判定
	bool IsHit(const std::string& name, const Vector2& pos) const;

#pragma region アクセサ
	Sprite::CenterAlignment GetCenterAlignment(const std::string& name) const { return _sprites.at(name).GetCenterAlignment(); }
	const Vector2& GetPosition(const std::string& name)const { return _sprites.at(name).GetPosition(); }
	const Vector2& GetScale(const std::string& name)const { return _sprites.at(name).GetScale(); }
	const Vector2& GetTexPos(const std::string& name)const { return _sprites.at(name).GetTexPos(); }
	const Vector2& GetTexSize(const std::string& name)const { return _sprites.at(name).GetTexSize(); }
	const Vector2& GetCenter(const std::string& name)const { return _sprites.at(name).GetCenter(); }
	float          GetAngle(const std::string& name)const { return _sprites.at(name).GetAngle(); }
	const Vector4& GetColor(const std::string& name)const { return _sprites.at(name).GetColor(); }

	void SetCenterAlignment(const std::string& name, Sprite::CenterAlignment alignment) {
		_sprites.at(name).SetCenterAlignment(alignment);
		_sprites.at(name).RecalcCenter(alignment);
	}
	void SetPosition(const std::string& name, const Vector2& p) { _sprites.at(name).SetPosition(p); }
	void SetScale(const std::string& name, const Vector2& s) { _sprites.at(name).SetScale(s); }
	void SetTexPos(const std::string& name, const Vector2& p) { _sprites.at(name).SetTexPos(p); }
	void SetTexSize(const std::string& name, const Vector2& s) { _sprites.at(name).SetTexSize(s); }
	void SetCenter(const std::string& name, const Vector2& c) { _sprites.at(name).SetCenter(c); }
	void SetAngle(const std::string& name, float a) { _sprites.at(name).SetAngle(a); }
	void SetColor(const std::string& name, const Vector4& c) { _sprites.at(name).SetColor(c); }
#pragma endregion

protected:
	// スプライト描画
	virtual void SpriteRender(
		const std::string& spriteName,
		const RenderContext& rc,
		const Vector2& offset,
		const Vector2& offsetScale);

private:
	std::unordered_map<std::string, Sprite> _sprites;
};