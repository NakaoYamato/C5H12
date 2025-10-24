#pragma once

#include "../../Source/Common/DamageableChild.h"

#include <functional>

class BodyPartController : public Component
{
public:
	BodyPartController() {}
	virtual ~BodyPartController() override = default;
	// 名前取得
	const char* GetName() const override { return "BodyPartController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// Gui描画
	void DrawGui() override;

#pragma region アクセサ
	// 初期化
	void Initialize(const std::string& name, float durability, float staggerInterval, int staggerToDownCount)
	{
		_bodyPartName = name;
		_maxDurability = durability;
		_currentDurability = durability;
		_staggerInterval = staggerInterval;
		_staggerToDownCount = staggerToDownCount;
	}
	// 部位破壊時のコールバック設定
	void SetOnDestroyCallback(const std::function<void()>& callback) {
		_onDestroyCallback = callback;
	}

	// 部位名取得
	const std::string& GetBodyPartName() const { return _bodyPartName; }
	// 最大部位耐久値取得
	float GetMaxDurability() const { return _maxDurability; }
	// 現在の部位耐久値取得
	float GetCurrentDurability() const { return _currentDurability; }
	// 怯み間隔取得
	float GetStaggerInterval() const { return _staggerInterval; }
	// 怯み回数取得
	int GetStaggerCount() const { return _staggerCount; }
	// 怯み判定
	bool IsStagger() const { return _isStagger; }
	// ダウン判定
	bool IsDown() const { return _isDown; }
	// 部位破壊判定
	bool IsDestroyed() const { return _isDestroyed; }

	// 最大部位耐久値設定
	void SetMaxDurability(float durability) { _maxDurability = durability; }
	// 現在の部位耐久値設定
	void SetCurrentDurability(float durability) { _currentDurability = durability; }
	// 怯み間隔設定
	void SetStaggerInterval(float interval) { _staggerInterval = interval; }
	// 怯み判定設定
	void SetIsStagger(bool isStagger) { _isStagger = isStagger; }
	// ダウン判定設定
	void SetIsDown(bool isDown) { _isDown = isDown; }
#pragma endregion
private:
	// 部位破壊時の処理
	void OnDestroyBodyPart();

private:
	std::weak_ptr<DamageableChild> _damageableChild;

	// 部位名
	std::string _bodyPartName;

	// 部位破壊時のコールバック
	std::function<void()> _onDestroyCallback;

	// 最大部位耐久値
	float _maxDurability = 100.0f;
	// 現在の部位耐久値
	float _currentDurability = 100.0f;
	// 部位破壊判定
	bool _isDestroyed = false;

	// 怯み間隔
	float _staggerInterval = 25.0f;
	// 怯みが発生した回数
	int _staggerCount = 0;
	// 怯み判定用ダメージ量
	float _staggerDamage = 0.0f;
	// 怯み判定
	bool _isStagger = false;

	// 何回怯んだらダウンするか(-1でダウンしない)
	int _staggerToDownCount = 3;
	// ダウン判定
	bool _isDown = false;

	// 前フレームの総ダメージ量
	float _prevTotalDamage = 0.0f;
};