#pragma once

#include "../../Library/Actor/Actor.h"
#include "../../Library/Scene/Scene.h"

#include "../../Library/2D/Sprite.h"
#include "../../Library/Math/Easing.h"

#include <unordered_map>

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
    /// <summary>
    /// UI描画
    /// </summary>
    /// <param name="rc"></param>
    /// <param name="offset"></param>
    /// <param name="offsetScale"></param>
    virtual void DrawUI(
        const RenderContext& rc,
        const Vector2& offset,
        const Vector2& offsetScale) = 0;

    // GUI描画
    virtual void DrawGui() override;

    // 画像読み込み
    void LoadTexture(const std::string& spriteName, const wchar_t* filename, CenterAlignment alignment);

#pragma region アクセサ
    CenterAlignment GetCenterAlignment(const std::string& name) const   { return _spriteDatas.at(name).centerAlignment; }
    const Vector2&  GetPosition(const std::string& name)const           { return _spriteDatas.at(name).position; }
    const Vector2&  GetScale(const std::string& name)const              { return _spriteDatas.at(name).scale; }
    const Vector2&  GetTexPos(const std::string& name)const             { return _spriteDatas.at(name).texPos; }
    const Vector2&  GetTexSize(const std::string& name)const            { return _spriteDatas.at(name).texSize; }
    const Vector2&  GetCenter(const std::string& name)const             { return _spriteDatas.at(name).center; }
    float           GetAngle(const std::string& name)const              { return _spriteDatas.at(name).angle; }
    const Vector4&  GetColor(const std::string& name)const              { return _spriteDatas.at(name).color; }

    void SetCenterAlignment(const std::string& name, CenterAlignment alignment) {
        _spriteDatas.at(name).centerAlignment = alignment;
        _spriteDatas.at(name).RecalcCenter(alignment);
    }
    void SetPosition(const std::string& name, const Vector2& p)  { _spriteDatas.at(name).position = p; }
    void SetScale(const std::string& name, const Vector2& s)     { _spriteDatas.at(name).scale = s; }
    void SetTexPos(const std::string& name, const Vector2& p)    { _spriteDatas.at(name).texPos = p; }
    void SetTexSize(const std::string& name, const Vector2& s)   { _spriteDatas.at(name).texSize = s; }
    void SetCenter(const std::string& name, const Vector2& c)    { _spriteDatas.at(name).center = c; }
    void SetAngle(const std::string& name, float a)              { _spriteDatas.at(name).angle = a; }
    void SetColor(const std::string& name, const Vector4& c)     { _spriteDatas.at(name).color = c; }
#pragma endregion

private:

protected:
    // スプライト描画
    virtual void SpriteRender(
		const std::string& spriteName,
        const RenderContext& rc, 
        const Vector2& offset,
        const Vector2& offsetScale);

protected:
    std::unordered_map<std::string, SpriteData> _spriteDatas;
};