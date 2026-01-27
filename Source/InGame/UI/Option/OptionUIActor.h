#pragma once

#include "../../Library/Actor/UI/Menu/MenuUIActor.h"

class OptionUIActor : public MenuUIActor
{
public:
	OptionUIActor() = default;
	~OptionUIActor() override {}
	// 生成時処理
	void OnCreate() override;
	// 起動フラグが変化したときの処理
	void OnChangedActive(bool isActive) override;
};

// プレイヤーの装備UIウィジェット
class PlayerEquipmentMenuWidget : public MenuWidget
{
public:
	PlayerEquipmentMenuWidget(std::string name) : MenuWidget(name) {}
	~PlayerEquipmentMenuWidget() override {}

	// ウィジェット名取得
	virtual std::string GetWidgetName() const { return ClassToString<PlayerEquipmentMenuWidget>(); }

};

// プレイヤーのステータスUIウィジェット
class PlayerStatusMenuWidget : public MenuWidget
{
public:
	PlayerStatusMenuWidget(std::string name) : MenuWidget(name) {}
	~PlayerStatusMenuWidget() override {}

	// ウィジェット名取得
	virtual std::string GetWidgetName() const { return ClassToString<PlayerStatusMenuWidget>(); }
};

// ゲーム設定UIウィジェット
class GameSettingsMenuWidget : public MenuWidget
{
public:
	GameSettingsMenuWidget(std::string name) : MenuWidget(name) {}
	~GameSettingsMenuWidget() override {}

	// ウィジェット名取得
	virtual std::string GetWidgetName() const { return ClassToString<GameSettingsMenuWidget>(); }
};