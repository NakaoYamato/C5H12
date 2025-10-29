#pragma once

#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Scene/Scene.h"

#include "../../Library/Component/SpriteRenderer.h"

#include <unordered_map>

// 前方宣言
class UIController;

/// <summary>
/// 2D描画を管理するメディエーター
/// </summary>
class CanvasMediator : public UIActor
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


class UIController : public SpriteRenderer
{
public:
    // 3D描画後の描画処理
    // ※メディエーターで制御するため処理しない
    void DelayedRender(const RenderContext& rc) override {}
    /// <summary>
    /// UI描画
    /// </summary>
    /// <param name="rc"></param>
    /// <param name="offset"></param>
    /// <param name="offsetScale"></param>
    virtual void DrawUI(const RenderContext& rc) = 0;
};