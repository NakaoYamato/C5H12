#pragma once

#include "../../Source/Common/Damageable.h"
#include "../../Source/Common/DamageSender.h"

class BuffController : public Component
{
public:
	enum BuffType : int
	{
		Attack = 0,
		Defense,
	};
	struct BuffData
	{
		BuffData(BuffType type, float duration, float value)
			: _type(type), _duration(duration), _value(value)
		{
		}
		BuffType _type;
		float _duration;
		float _value;
	};

public:
	BuffController() {}
	virtual ~BuffController() override = default;

	// 名前取得
	const char* GetName() const override { return "BuffController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// Gui描画
	void DrawGui() override;

	// バフ追加
	void AddBuff(BuffType _type, float duration, float _value);

	std::vector<std::unique_ptr<BuffData>>& GetActiveBuffs() { return _activeBuffs; }
private:
	std::vector<std::unique_ptr<BuffData>> _activeBuffs;
	std::weak_ptr<Damageable> _damageable;
	std::weak_ptr<DamageSender> _damageSender;

	float _totalAttackBuff = 0.0f;
	float _totalDefenseBuff = 0.0f;
};