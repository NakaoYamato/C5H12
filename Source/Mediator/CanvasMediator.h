#pragma once

#include "../../Library/Actor/Actor.h"
#include "../../Library/Scene/Scene.h"

#include "../../Library/2D/Sprite.h"

// 前方宣言
class UIController;

/// <summary>
/// 2D描画を管理するメディエーター
/// </summary>
class CanvasMediator : public Actor
{
public:
	~CanvasMediator() override {};

    // 生成時処理
    void OnCreate() override;
    // 開始時処理
    void OnStart() override;
    // 更新前処理
    void OnPreUpdate(float elapsedTime) override;
    // 遅延更新処理
    void OnLateUpdate(float elapsedTime) override;
    // 固定間隔更新処理
    void OnFixedUpdate() override;
	// UI描画処理
	void OnDelayedRender(const RenderContext& rc) override;
    // GUI描画
    void OnDrawGui() override;

    /// <summary>
    /// ユーザーが操作するプレイヤーのHPUI設定
    /// </summary>
    /// <param name="healthUIController"></param>
    void SetUserHealthUI(UIController* healthUIController)
    {
		_userHealthUI = healthUIController;
    }
	/// <summary>
	/// ユーザーが操作しないプレイヤーのHPUI追加
	/// </summary>
	/// <param name="healthUIController"></param>
	void AddOtherUserHealthUI(UIController* healthUIController)
	{
		_otherUserHealthUI.push_back(healthUIController);
	}
	/// <summary>
	/// ユーザーが操作しないプレイヤーのHPUI削除
	/// </summary>
	/// <param name="healthUIController"></param>
	void RemoveOtherUserHealthUI(UIController* healthUIController)
	{
		auto it = std::remove(_otherUserHealthUI.begin(), _otherUserHealthUI.end(), healthUIController);
		if (it != _otherUserHealthUI.end())
		{
			_otherUserHealthUI.erase(it);
		}
	}

private:
	// ユーザーが操作するプレイヤーのHPUI
	UIController* _userHealthUI = nullptr;
    // ユーザーが操作しないプレイヤーのHPUIリスト
	std::vector<UIController*> _otherUserHealthUI;

    // HPUIの間隔
	Vector2 _healthUIInterval = Vector2(0.0f, 100.0f);
    // HPUIのスケール
	Vector2 _healthUIScale = Vector2(0.5f, 0.5f);
};


class UIController : public Component
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

public:
    /// <summary>
    /// UI描画
    /// </summary>
    /// <param name="rc"></param>
    /// <param name="offset"></param>
    /// <param name="offsetScale"></param>
    virtual void DrawUI(
        const RenderContext& rc,
        const Vector2& offset,
        const Vector2& offsetScale) 
    {
		SpriteRender(rc, offset, offsetScale);
    };

    // GUI描画
    virtual void DrawGui() override;

    // 画像読み込み
    void LoadTexture(const wchar_t* filename, CenterAlignment alignment);

#pragma region アクセサ
    CenterAlignment GetCenterAlignment() const { return _centerAlignment; }
    const Vector2& GetPosition()const   { return _position; }
    const Vector2& GetScale()const      { return _scale; }
    const Vector2& GetTexPos()const     { return _texPos; }
    const Vector2& GetTexSize()const    { return _texSize; }
    const Vector2& GetCenter()const     { return _center; }
    float GetAngle()const               { return _angle; }
    const Vector4& GetColor()const      { return _color; }

    void SetCenterAlignment(CenterAlignment alignment) {
        _centerAlignment = alignment;
        RecalcCenter(alignment);
    }
    void SetPosition(const Vector2& p)  { _position = p; }
    void SetScale(const Vector2& s)     { _scale = s; }
    void SetTexPos(const Vector2& p)    { _texPos = p; }
    void SetTexSize(const Vector2& s)   { _texSize = s; }
    void SetCenter(const Vector2& c)    { _center = c; }
    void SetAngle(float a)              { _angle = a; }
    void SetColor(const Vector4& c)     { _color = c; }
#pragma endregion

private:
	// 中心位置を再計算
    void RecalcCenter(CenterAlignment alignment);

protected:
    // スプライト描画
    virtual void SpriteRender(
        const RenderContext& rc, 
        const Vector2& offset,
        const Vector2& offsetScale);

protected:
    std::unique_ptr<Sprite> _sprite;

	CenterAlignment _centerAlignment = CenterAlignment::CENTER_CENTER;
    Vector2 _position{};
    Vector2 _scale{ 1.0f, 1.0f };
    Vector2 _texPos{};
    Vector2 _texSize{};
    Vector2 _center{};
    float   _angle{};
    Vector4 _color{ 1,1,1,1 };
};