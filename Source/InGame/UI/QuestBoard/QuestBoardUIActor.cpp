#include "QuestBoardUIActor.h"

#include "../../Library/Scene/Scene.h"

#include <Mygui.h>

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

// 開始処理
void QuestBoardUIWidget::OnEnter()
{
	// 確認状態解除
	_isConfirmingQuest = false;
	// 選択肢の変更可能にする
	SetCanChangeIndex(true);
}

// 描画処理
void QuestBoardUIWidget::Render(const RenderContext& rc, MenuUIActor* owner)
{
	auto questManager = _questManager.lock();
	if (!questManager)
		return;

	TextRenderer& textRenderer = owner->GetScene()->GetTextRenderer();
	RectTransform rect = _rectTransform;
	size_t i = 0;
	for (auto& questData : questManager->GetQuestDataList())
	{
		if (questData.questType != _displayQuestType)
			continue;

		// 子要素のトランスフォーム設定
		_optionSprite.UpdateTransform(&rect);
		// 背景描画
		_optionSprite.Render(rc, owner->GetScene()->GetTextureRenderer());

		// 選択中処理
		if (i == _selectedOptionIndex)
		{
			// 子要素のトランスフォーム設定
			_selectedOptionSprite.UpdateTransform(&rect);
			// 背景描画
			_selectedOptionSprite.Render(rc, owner->GetScene()->GetTextureRenderer());

			// 説明文背景
			_descriptionSprite.UpdateTransform(nullptr);
			_descriptionSprite.Render(rc, owner->GetScene()->GetTextureRenderer());

			// 説明文描画
			std::wstring text = ToUtf16(questData.description);
			textRenderer.Draw(
				FontType::MSGothic,
				text.c_str(),
				_descriptionLabelOffset,
				_descriptionLabelColor,
				0.0f,
				Vector2::Zero,
				_descriptionFontSize);
		}

		// ラベル描画
		std::wstring text = ToUtf16(questData.name);
		Vector2 labelPos = rect.GetWorldPosition() + _optionLabelOffset;
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

	if (_isConfirmingQuest)
	{
		// ラベル描画
		std::wstring text = L"クエストを受注しますか?";
		textRenderer.Draw(
			FontType::MSGothic,
			text.c_str(),
			_confirmLabelOffset,
			Vector4::White,
			0.0f,
			Vector2::Zero,
			_confirmFontSize);
	}
}

// GUI描画処理
void QuestBoardUIWidget::DrawGui(MenuUIActor* owner)
{
	MenuWidget::DrawGui(owner);
	if (ImGui::TreeNode(u8"説明文"))
	{
		_descriptionSprite.DrawGui();
		ImGui::DragFloat2(u8"説明文オフセット", &_descriptionLabelOffset.x);
		ImGui::DragFloat2(u8"説明文フォントサイズ", &_descriptionFontSize.x);
		ImGui::ColorEdit4(u8"説明文色", &_descriptionLabelColor.x);

		ImGui::Checkbox(u8"クエスト選択確認中", &_isConfirmingQuest);

		ImGui::DragFloat2(u8"確認文オフセット", &_confirmLabelOffset.x);
		ImGui::DragFloat2(u8"確認文フォントサイズ", &_confirmFontSize.x);

		ImGui::TreePop();
	}
}

// 選択肢選択処理
void QuestBoardUIWidget::SelectOption(MenuUIActor* owner)
{
	if (_isConfirmingQuest)
	{
		// すでに確認中していて、さらに選択された場合はクエスト受注
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
		// 選択肢の変更可能にする
		SetCanChangeIndex(true);
	}
	else
	{
		// まだ確認中でない場合は確認状態にする
		_isConfirmingQuest = true;
		// 選択肢の変更不可能にする
		SetCanChangeIndex(false);
	}
}

// 戻る選択肢選択処理
void QuestBoardUIWidget::BackOption(MenuUIActor* owner)
{
	if (_isConfirmingQuest)
	{
		// 確認中の場合は確認状態解除
		_isConfirmingQuest = false;
		// 選択肢の変更可能にする
		SetCanChangeIndex(true);
	}
	else
	{
		// 確認中でない場合はメニュー閉じる
		MenuWidget::BackOption(owner);
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

#pragma region ファイル
// ファイル読み込み処理
void QuestBoardUIWidget::OnLoadFromFile(nlohmann::json* json)
{
	{
		auto& sprJosn = (*json)["descriptionSprite"];
		if (!sprJosn.is_null())
		{
			// テクスチャデータ
			std::string textureFilePath = sprJosn.value("textureFilePath", "");
			_descriptionSprite.LoadTexture(ToWString(textureFilePath).c_str(),
				static_cast<Sprite::CenterAlignment>(sprJosn.value("centerAlignment", Sprite::CenterAlignment::CenterCenter)));
			// トランスフォームデータ
			_descriptionSprite.GetRectTransform() = sprJosn.value("RectTransform", RectTransform());
			// マテリアルデータ
			_descriptionSprite.GetMaterial().LoadFromFile(sprJosn);
			_descriptionSprite.SetCenterAlignment(sprJosn.value("centerAlignment", Sprite::CenterAlignment::CenterCenter));
			_descriptionSprite.SetTexPos(sprJosn.value("texPos", _descriptionSprite.GetTexPos()));
			_descriptionSprite.SetTexSize(sprJosn.value("texSize", _descriptionSprite.GetTexSize()));
			_descriptionSprite.SetCenter(sprJosn.value("center", _descriptionSprite.GetCenter()));
			_descriptionSprite.SetColor(sprJosn.value("color", _descriptionSprite.GetColor()));
			_descriptionSprite.SetDepthState(sprJosn.value("depthState", DepthState::TestAndWrite));
			_descriptionSprite.SetStencil(sprJosn.value("stencil", 0));
		}
	}
	_descriptionLabelOffset = (*json).value("descriptionLabelOffset", Vector2(50.0f, 10.0f));
	_descriptionFontSize = (*json).value("descriptionFontSize", Vector2(0.5f, 0.5f));
	_descriptionLabelColor = (*json).value("descriptionLabelColor", Vector4(1.0f, 1.0f, 1.0f, 1.0f));

	_confirmLabelOffset = (*json).value("confirmLabelOffset", Vector2(50.0f, 200.0f));
	_confirmFontSize = (*json).value("confirmFontSize", Vector2(0.5f, 0.5f));
}

// ファイル保存処理
void QuestBoardUIWidget::OnSaveToFile(nlohmann::json* json)
{
	{
		auto& sprJosn = (*json)["descriptionSprite"];
		// テクスチャデータ
		sprJosn["textureFilePath"] = ToString(_descriptionSprite.GetTexture().GetFilepath());

		// トランスフォームデータ
		sprJosn["RectTransform"] = _descriptionSprite.GetRectTransform();

		// マテリアルデータ
		_descriptionSprite.GetMaterial().SaveToFile(sprJosn);

		sprJosn["centerAlignment"] = _descriptionSprite.GetCenterAlignment();
		sprJosn["texPos"] = _descriptionSprite.GetTexPos();
		sprJosn["texSize"] = _descriptionSprite.GetTexSize();
		sprJosn["center"] = _descriptionSprite.GetCenter();
		sprJosn["color"] = _descriptionSprite.GetColor();
		sprJosn["depthState"] = _descriptionSprite.GetDepthState();
		sprJosn["stencil"] = _descriptionSprite.GetStencil();
	}
	(*json)["descriptionLabelOffset"] = _descriptionLabelOffset;
	(*json)["descriptionFontSize"] = _descriptionFontSize;
	(*json)["descriptionLabelColor"] = _descriptionLabelColor;

	(*json)["confirmLabelOffset"] = _confirmLabelOffset;
	(*json)["confirmFontSize"] = _confirmFontSize;
}
#pragma endregion
