#pragma once

#include "../../Library/Actor/Actor.h"
#include "../../Library/2D/Sprite.h"
#include "../../Library/Scene/Scene.h"

#include <unordered_map>

class TitleItemBase;
using TitleItemRef = std::shared_ptr<TitleItemBase>;

class TitleMediator : public Actor
{
public:
	struct CommandData
	{
		// 送信者
		std::string sender;
		// 対象
		std::string target;
		// コマンドの種類
		std::string command;
		// 遅延時間（秒）
		float delayTime = 0.0f;
	};

#pragma region コマンド名
	static const char* ActivateCommand;
	static const char* DeactivateCommand;
#pragma endregion

#pragma region アイテム名
	static const char* AllItem;

	static const char* ToHomeItem;
	static const char* ToOnlineItem;
	static const char* ToOfflineItem;
	static const char* ToSettingItem;
	static const char* ToExitItem;
	static const char* ToOnlineSettingItem;
#pragma endregion


public:
	~TitleMediator() override {}
	// 生成時処理
	void OnCreate() override;
	// 遅延更新処理
	void OnLateUpdate(float elapsedTime) override;
	// UI描画処理
	void OnDelayedRender(const RenderContext& rc) override;
	// GUI描画
	void OnDrawGui() override;

	// コマンドを受信
	void ReceiveCommand(const std::string& sender, const std::string& target, const std::string& command, float delayTime = 0.0f)
	{
		CommandData commandData;
		commandData.sender = sender;
		commandData.target = target;
		commandData.command = command;
		commandData.delayTime = delayTime;
		_commandList.push_back(commandData);
	}
	// タイトルアイテムの登録
	void RegisterTitleItem(TitleItemRef item)
	{
		_titleItems.push_back(item);
	}
	// タイトルアイテムの削除
	void UnregisterTitleItem(TitleItemRef item)
	{
		auto it = std::remove(_titleItems.begin(), _titleItems.end(), item);
		if (it != _titleItems.end())
		{
			_titleItems.erase(it, _titleItems.end());
		}
	}
private:
	// コマンドを実行
	void ExecuteCommand(const CommandData& command);

private:
	// タイトルアイテムのリスト
	std::vector<TitleItemRef> _titleItems;
	// コマンドリスト
	std::vector<CommandData> _commandList;
};

class TitleItemBase
{
public:
	TitleItemBase() = delete;
	TitleItemBase(TitleMediator* titleMediator, const std::string& itemName)
		: _titleMediator(titleMediator), _titleName(itemName) {
	}
	virtual ~TitleItemBase() {}

	// 更新処理
	virtual void Update(float elapsedTime) {}
	// 描画
	virtual void Render(Scene* scene, const RenderContext& rc) = 0;
	// GUI描画
	virtual void DrawGui();
	// コマンドを実行
	virtual void ExecuteCommand(const std::string& command);

	// タイトルアイテム名を取得
	const std::string& GetName() const
	{
		return _titleName;
	}
protected:
	// タイトルメディエーターへの参照
	TitleMediator* _titleMediator = nullptr;
	// タイトルアイテム名
	std::string _titleName;
	// スプライトのマップ
	std::unordered_map<std::string, Sprite> _sprites;
	// アクティブフラグ
	bool _isActive = true;
};