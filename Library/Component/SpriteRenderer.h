#pragma once

#include "Component.h"
#include "../../Library/2D/Sprite.h"
#include "../../Library/Math/Easing.h"

#include <unordered_map>

/// <summary>
/// 画像描画コンポーネント
/// </summary>
class SpriteRenderer : public Component
{
public:
	enum CenterAlignment
	{
		LEFT_UP,
		LEFT_CENTER,
		LEFT_DOWN,
		CENTER_UP,
		CENTER_CENTER,
		CENTER_DOWN,
		RIGHT_UP,
		RIGHT_CENTER,
		RIGHT_DOWN,
	};
	struct SpriteData
	{
		std::unique_ptr<Sprite> sprite;
		CenterAlignment         centerAlignment = CenterAlignment::CENTER_CENTER;
		Vector2                 position{};
		Vector2                 scale{ 1.0f, 1.0f };
		Vector2                 texPos{};
		Vector2                 texSize{};
		Vector2                 center{};
		float                   angle{};
		Vector4                 color{ 1,1,1,1 };
		// 中心位置を再計算
		void RecalcCenter(CenterAlignment alignment);
	};
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
	void LoadTexture(const std::string& spriteName, const wchar_t* filename, CenterAlignment alignment);
	// 画像との当たり判定
	bool IsHit(const std::string& name, const Vector2& pos) const;

#pragma region アクセサ
	CenterAlignment GetCenterAlignment(const std::string& name) const { return _spriteDatas.at(name).centerAlignment; }
	const Vector2& GetPosition(const std::string& name)const { return _spriteDatas.at(name).position; }
	const Vector2& GetScale(const std::string& name)const { return _spriteDatas.at(name).scale; }
	const Vector2& GetTexPos(const std::string& name)const { return _spriteDatas.at(name).texPos; }
	const Vector2& GetTexSize(const std::string& name)const { return _spriteDatas.at(name).texSize; }
	const Vector2& GetCenter(const std::string& name)const { return _spriteDatas.at(name).center; }
	float          GetAngle(const std::string& name)const { return _spriteDatas.at(name).angle; }
	const Vector4& GetColor(const std::string& name)const { return _spriteDatas.at(name).color; }

	void SetCenterAlignment(const std::string& name, CenterAlignment alignment) {
		_spriteDatas.at(name).centerAlignment = alignment;
		_spriteDatas.at(name).RecalcCenter(alignment);
	}
	void SetPosition(const std::string& name, const Vector2& p) { _spriteDatas.at(name).position = p; }
	void SetScale(const std::string& name, const Vector2& s) { _spriteDatas.at(name).scale = s; }
	void SetTexPos(const std::string& name, const Vector2& p) { _spriteDatas.at(name).texPos = p; }
	void SetTexSize(const std::string& name, const Vector2& s) { _spriteDatas.at(name).texSize = s; }
	void SetCenter(const std::string& name, const Vector2& c) { _spriteDatas.at(name).center = c; }
	void SetAngle(const std::string& name, float a) { _spriteDatas.at(name).angle = a; }
	void SetColor(const std::string& name, const Vector4& c) { _spriteDatas.at(name).color = c; }
#pragma endregion

protected:
	// スプライト描画
	virtual void SpriteRender(
		const std::string& spriteName,
		const RenderContext& rc,
		const Vector2& offset,
		const Vector2& offsetScale);

private:
	std::unordered_map<std::string, SpriteData> _spriteDatas;
};