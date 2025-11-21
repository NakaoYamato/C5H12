#pragma once

#include "../../Library/Component/Component.h"

class OperateUIController : public Component
{
public:
	struct DescriptionData
	{
		// 起動フラグ
		bool isActive = true;
		// 生存時間
		float lifeTime = 0.2f;
		// 入力アクション名群
		std::vector<std::string> inputActionNames = {};
	};

public:
	OperateUIController() {}
	~OperateUIController() override {}
	// 名前取得
	const char* GetName() const override { return "OperateUIController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

	// 説明文追加
	void AddDescription(
		const std::string& description,
		const std::vector<std::string>& inputActionNames);

private:
	std::unordered_map<std::string, DescriptionData> _descriptionMap;

	Vector2 _descPosition = Vector2(1630.0f, 40.0f);
	Vector2 _descScale = Vector2(0.8f, 0.8f);
	Vector2 _descOrigin = Vector2(0.38f, 0.0f);
	float _descInterval = 35.0f;
	float _inputUIInterval = 100.0f;
	float _orInterval = -15.0f;

	Vector2 _inputUIOffset = Vector2(80.0f, 12.5f);
	Vector2 _inputUIScale = Vector2(0.35f, 0.35f);
};