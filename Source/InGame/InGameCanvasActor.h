#pragma once

#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Scene/Scene.h"

#include "../../Library/Component/SpriteRenderer.h"

class InGameCanvasActor : public UIActor
{
public:
	InGameCanvasActor() = default;
	~InGameCanvasActor() override {};

	// 生成時処理
	void OnCreate() override;
	// GUI描画
	void OnDrawGui() override;

	// プレイヤー設定
	void RegisterPlayerActor(Actor* playerActor);

	// UI作成
	template<class T, class... Args>
	std::shared_ptr<T> CreateUIActor(const std::string& name, Args... args)
	{
		auto actor = GetScene()->RegisterActor<T>(name, ActorTag::UI, args...);
		actor->SetParent(this);
		_uiActiveMap[name] = true;
		return actor;
	}

	// UI名前取得
	std::vector<std::string> GetUIActorNames() const;

	// UI表示設定
	void SetUIActive(const std::string& name, bool isActive);

private:
	// 各UIの表示フラグ　アクターの有効無効とは別
	std::unordered_map<std::string, bool> _uiActiveMap;
};