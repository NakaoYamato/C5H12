#pragma once

#include "../../Library/Actor/Actor.h"
#include "../../Library/2D/Sprite.h"
#include "../../Library/Scene/Scene.h"

#include <unordered_map>

#pragma region 前方宣言
class TitleItemBase;
using TitleItemRef = std::shared_ptr<TitleItemBase>;
#pragma endregion

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

#pragma region 対象
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
	void ReceiveCommand(const std::string& sender, const std::string& target, const std::string& command, float delayTime = 0.0f);
	// タイトルアイテムの登録
	void RegisterTitleItem(TitleItemRef item);
	// タイトルアイテムの削除
	void UnregisterTitleItem(TitleItemRef item);
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
	virtual void ExecuteCommand(const TitleMediator::CommandData& commandData);

	// タイトルアイテム名を取得
	const std::string& GetName() const { return _titleName; }
	// タイトルアイテムがアクティブかどうか
	bool IsActive() const {	return _isActive; }
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