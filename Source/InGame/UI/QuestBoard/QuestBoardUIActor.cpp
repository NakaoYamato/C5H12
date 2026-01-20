#include "QuestBoardUIActor.h"

#include "../../Library/Scene/Scene.h"

// 生成時処理
void QuestBoardUIActor::OnCreate()
{
	MenuUIActor::OnCreate();

	GetRectTransform().SetLocalPosition(Vector2(200.0f, 200.0f));

	// ゲームマネージャーからクエスト受注コントローラー取得
	auto gameManager = GetScene()->GetActorManager().FindByName("GameManager", ActorTag::System);
	if (gameManager)
	{
		if (auto cont = gameManager->GetComponent<QuestOrderController>())
		{
			_questOrderController = cont;
		}
	}

	// コールバック登録
	RegisterOptionSelectedCallback("PopPage", [this](MenuUIActor* owner) -> void
		{
			owner->PopPage();
		});

	// ウィジェット登録
	std::shared_ptr<QuestBoardUIWidget> normalQuest = std::make_shared<QuestBoardUIWidget>("NormalQuest");
	normalQuest->SetDisplayQuestType(QuestData::QuestType::Normal);
	RegisterWidget(normalQuest);
	std::shared_ptr<QuestBoardUIWidget> eventQuest = std::make_shared<QuestBoardUIWidget>("EventQuest");
	eventQuest->SetDisplayQuestType(QuestData::QuestType::Event);
	RegisterWidget(eventQuest);

	SetFilePath("./Data/Resource/Actor/QuestBoardUIActor/MenuUI.json");
	LoadFromFile();
}

// クエスト受注
// 成功でtrue
bool QuestBoardUIActor::AcceptQuest(int questIndex)
{
	auto questOrderController = _questOrderController.lock();
	if (!questOrderController)
		return false;
	questOrderController->AcceptQuest(questIndex);
	return true;
}

QuestBoardUIWidget::QuestBoardUIWidget(std::string name) : MenuWidget(name)
{
	// クエストデータマネージャー取得
	_questManager = ResourceManager::Instance().GetResourceAs<QuestManager>("QuestManager");
}

// 描画処理
void QuestBoardUIWidget::Render(const RenderContext& rc, MenuUIActor* owner)
{
	auto questManager = _questManager.lock();
	if (!questManager)
		return;

	RectTransform rect = _rectTransform;
	size_t i = 0;
	for (auto& questData : questManager->GetQuestDataList())
	{
		if (questData.questType != _displayQuestType)
			continue;

		Sprite* spr = i == _selectedOptionIndex ?
			&_selectedOptionSprite :
			&_optionSprite;
		// 子要素のトランスフォーム設定
		spr->UpdateTransform(&rect);
		// 背景描画
		spr->Render(rc, owner->GetScene()->GetTextureRenderer());
		// ラベル描画
		std::wstring text = ToUtf16(questData.name);
		Vector2 labelPos = rect.GetWorldPosition() + _optionLabelOffset;
		TextRenderer& textRenderer = owner->GetScene()->GetTextRenderer();
		textRenderer.Draw(
			FontType::MSGothic,
			text.c_str(),
			labelPos,
			i == _selectedOptionIndex ? _optionLabelSelectedColor : _optionLabelColor,
			0.0f,
			Vector2::Zero,
			_optionFontSize);
		// 次の選択肢位置計算
		Vector2 position = rect.GetLocalPosition();
		position.y += _optionVerticalSpacing;
		rect.SetLocalPosition(position);
		rect.UpdateTransform(&owner->GetRectTransform());
		++i;
	}
}

// 選択肢選択処理
void QuestBoardUIWidget::SelectOption(MenuUIActor* owner)
{
	auto questBoardUIActor = dynamic_cast<QuestBoardUIActor*>(owner);
	if (!questBoardUIActor)
		return;

	// クエスト選択処理呼び出し
	if (questBoardUIActor->AcceptQuest(static_cast<int>(_selectedOptionIndex)))
	{
		// 受注成功したらメニュー閉じる
		owner->PopAllPages();
		return;
	}
}

// インデックス範囲制限
size_t QuestBoardUIWidget::ClampSelectedOptionIndex(size_t index)
{
	auto questManager = _questManager.lock();
	if (!questManager)
		return 0;

	// 何かしらの要因でクエスト数が変化する場合を考慮して毎回計算
	size_t i = 0;
	for (auto& questData : questManager->GetQuestDataList())
	{
		if (questData.questType != _displayQuestType)
			continue;

		++i;
	}
	if (i == 0)
		return 0;

	index = index % (static_cast<int>(i));
	if (index < 0)
		index += static_cast<int>(i);
	return index;
}
