#pragma once

#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Component/SpriteRenderer.h"

#include "../../Source/Quest/QuestController.h"

class PlayerQuestOrderUIController : public Component
{
public:
    PlayerQuestOrderUIController() = default;
    ~PlayerQuestOrderUIController() override {}
    // 名前取得
    const char* GetName() const override { return "PlayerQuestOrderUIController"; }
    // 開始処理
    void Start() override;
    // 更新処理
    void Update(float elapsedTime) override;
    // GUI描画
    void DrawGui() override;

	void SetPlayerActor(std::shared_ptr<Actor> playerActor) { _playerActor = playerActor; }
private:
    std::weak_ptr<SpriteRenderer> _spriteRenderer;
    std::weak_ptr<Actor> _playerActor;
	std::weak_ptr<QuestController> _questController;

    // 各種スプライトの名前
    const std::string FrameSpr = "Frame";

    // オフセット
	float _offsetY = 2.1f;
};