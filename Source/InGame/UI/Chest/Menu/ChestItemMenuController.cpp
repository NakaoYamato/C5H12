#include "ChestItemMenuController.h"

#include "../ChestUIController.h"
#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Algorithm/Converter.h"

#include <imgui.h>

// 開始処理
void ChestItemMenuController::Start()
{
    _userDataManager = ResourceManager::Instance().GetResourceAs<UserDataManager>("UserDataManager");

    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();
    if (auto spriteRenderer = _spriteRenderer.lock())
	{
		if (!spriteRenderer->IsLoaded())
		{
			spriteRenderer->LoadTexture(PourchBackSpr, L"");
			spriteRenderer->LoadTexture(StrageBackSpr, L"");
			spriteRenderer->LoadTexture(TextBoxSpr, L"");
		}
	}

	// アイテムの前面スプライト
	_itemFrontSprite = std::make_unique<Sprite>(L"Data/Texture/UI/Chest/ItemFront.png", Sprite::LeftUp);
	_itemFrontSprite->GetRectTransform().SetReflectParentScale(false);
	_itemFrontSprite->GetRectTransform().SetLocalScale(Vector2(0.4f, 0.4f));
	// アイテムの背景スプライト
	_itemBackSprite = std::make_unique<Sprite>(L"Data/Texture/UI/Chest/ItemBack.png", Sprite::LeftUp);
	// プロパティ表示用スプライト
	_propertySprite = std::make_unique<Sprite>(L"", Sprite::LeftUp);
	_propertySprite->GetRectTransform().SetReflectParentScale(false);
	_propertySprite->SetColor(Vector4::Black);
	_propertySprite->SetColorAlpha(0.5f);

	if (!LoadFromFile())
	{
		// ポーチ内アイテム用トランスフォーム初期化
		_pourchItemTransform.SetLocalPosition(Vector2(0.0f, 7.5f));
		_pourchItemTransform.SetLocalScale(Vector2(0.4f, 0.4f));
		_pourchItemTransform.SetReflectParentScale(false);
	}
	_pourchText.text = L"アイテムポーチ";
	_strageText.text = L"アイテムボックス";

	// 入力UI
	_selectInputUI.keyboardKey = 'F';
	_selectInputUI.gamePadKey = XINPUT_GAMEPAD_A;
	_backInputUI.keyboardKey = VK_ESCAPE;
	_backInputUI.gamePadKey = XINPUT_GAMEPAD_B;
	_tabChangeInputUI.keyboardKey = 'Q';
	_tabChangeInputUI.gamePadKey = XINPUT_GAMEPAD_RIGHT_THUMB;
	_sortInputUI.keyboardKey = VK_SHIFT;
	_sortInputUI.gamePadKey = XINPUT_GAMEPAD_LEFT_THUMB;
	_selectInputUIText.text = L"：選択";
	_backInputUIText.text = L"：戻る";
	_tabChangeInputUIText.text = L"：タブ切替";
	_sortInputUIText.text = L"：ポーチ整列";
}

// 更新処理
void ChestItemMenuController::Update(float elapsedTime)
{
	auto spriteRenderer = _spriteRenderer.lock();
	if (!spriteRenderer)
		return;
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;

	// 選択中のタブに合わせて背景トランスフォーム、説明テキスト更新
	_itemName.text = L"";
	_itemDescription.text = L"";
	_itemQuantityText.text = L"";
	Vector2 localPosition = Vector2::Zero;
	int pouchItemQuantity = 0;
	int strageItemQuantity = 0;
	switch (_tab)
	{
	case ChestItemMenuController::Tab::Pourch:
		// 選択している背景を明るくする
		spriteRenderer->SetColorAlpha(PourchBackSpr, 0.5f);
		spriteRenderer->SetColorAlpha(StrageBackSpr, 0.25f);
		// テキストも更新
		_pourchText.color = Vector4::White;
		_strageText.color = Vector4::Gray;

		localPosition = _pourchItemTransform.GetLocalPosition() + _pourchItemOffset * static_cast<float>(_currentIndex);
		_itemFrontSprite->GetRectTransform().SetLocalPosition(localPosition);
		_itemFrontSprite->UpdateTransform(&spriteRenderer->GetRectTransform(PourchBackSpr));

		{
			auto pouchItem = &userDataManager->GetPouchItems()[_currentIndex];
			if (!pouchItem)
				break;
			auto itemData = userDataManager->GetAcquiredItemData(pouchItem->itemIndex);
			if (!itemData)
				break;
			auto baseData = itemData->GetBaseData();
			if (!baseData)
				break;

			_itemName.text = ToUtf16(baseData->name);
			_itemDescription.text = ToUtf16(baseData->description);
			pouchItemQuantity = pouchItem->quantity;
			strageItemQuantity = itemData->quantity - pouchItemQuantity;
		}
		break;
	case ChestItemMenuController::Tab::Strage:
		// 選択している背景を明るくする
		spriteRenderer->SetColorAlpha(PourchBackSpr, 0.25f);
		spriteRenderer->SetColorAlpha(StrageBackSpr, 0.5f);
		// テキストも更新
		_strageText.color = Vector4::White;
		_pourchText.color = Vector4::Gray;

		localPosition = _strageItemTransform.GetLocalPosition();
		localPosition.x += (_currentIndex % _strageItemColumnIndex) * _strageItemOffset.x;
		localPosition.y += (_currentIndex / _strageItemColumnIndex) * _strageItemOffset.y;
		_itemFrontSprite->GetRectTransform().SetLocalPosition(localPosition);
		_itemFrontSprite->UpdateTransform(&spriteRenderer->GetRectTransform(StrageBackSpr));

		{
			auto iter = userDataManager->GetAcquiredItemDataMap().find(_currentIndex);
			if (iter == userDataManager->GetAcquiredItemDataMap().end())
				break;
			auto baseData = iter->second.GetBaseData();
			if (!baseData)
				break;

			_itemName.text = ToUtf16(baseData->name);
			_itemDescription.text = ToUtf16(baseData->description);

			auto pouchItem = userDataManager->GetPouchItemFromItemIndex(iter->first);
			pouchItemQuantity = pouchItem ? pouchItem->quantity : 0;
			strageItemQuantity = iter->second.quantity - pouchItemQuantity;
		}
		break;
	}
	_itemQuantityText.text = L"ポーチ内: " + std::to_wstring(pouchItemQuantity);
	_itemQuantityText.text += L"個\n";
	_itemQuantityText.text += L"ボックス内: " + std::to_wstring(strageItemQuantity);
	_itemQuantityText.text += L"個";

	// ポーチ内アイテム用トランスフォーム更新
	_pourchItemTransform.UpdateTransform(&spriteRenderer->GetRectTransform(PourchBackSpr));
	// ストレージ内アイテム用トランスフォーム更新
	_strageItemTransform.UpdateTransform(&spriteRenderer->GetRectTransform(StrageBackSpr));
}

// 3D描画後の描画処理
void ChestItemMenuController::DelayedRender(const RenderContext& rc)
{
	auto& textureRenderer = GetActor()->GetScene()->GetTextureRenderer();

	// ポーチメニュー描画
	RenderPourch(rc);

	// ストレージメニュー描画
	RenderStrage(rc);

	// アイテムの前面描画
	_itemFrontSprite->Render(rc, textureRenderer);

	// プロパティ表示
	if (_state == State::PropertySelecting)
	{
		auto spriteRenderer = _spriteRenderer.lock();
		if (!spriteRenderer)
			return;
		// プロパティ表示用スプライト描画
		_propertySprite->Render(rc, textureRenderer);
		Vector2 position = _propertySprite->GetRectTransform().GetWorldPosition();
		position.y += 10.0f;
		std::string allStr{}, oneStr{};
		// 選択肢描画
		switch (_tab)
		{
		case ChestItemMenuController::Tab::Pourch:
			allStr = "すべて戻す";
			oneStr = "一つ戻す";
			break;
		case ChestItemMenuController::Tab::Strage:
			allStr = "すべて移動";
			oneStr = "一つ移動";
			break;
		}
		GetActor()->GetScene()->GetTextRenderer().Draw(FontType::MSGothic,
			allStr.c_str(),
			position,
			_property == Property::MoveAll ? _propertySelectTextColor : _propertyTextColor,
			0.0f,
			_propertyTextOrigin,
			_propertyTextScale);
		position.y += _propertyTextInterval;
		GetActor()->GetScene()->GetTextRenderer().Draw(FontType::MSGothic,
			oneStr.c_str(),
			position,
			_property == Property::MoveOne ? _propertySelectTextColor : _propertyTextColor,
			0.0f,
			_propertyTextOrigin,
			_propertyTextScale);
		position.y += _propertyTextInterval;
		GetActor()->GetScene()->GetTextRenderer().Draw(FontType::MSGothic,
			"戻る",
			position,
			_property == Property::Cancel ? _propertySelectTextColor : _propertyTextColor,
			0.0f,
			_propertyTextOrigin,
			_propertyTextScale);

	}
	// 入力UI
	GetActor()->GetScene()->GetInputUI()->Draw(_selectInputUI);;
	GetActor()->GetScene()->GetTextRenderer().Draw(_selectInputUIText);
	GetActor()->GetScene()->GetInputUI()->Draw(_backInputUI);;
	GetActor()->GetScene()->GetTextRenderer().Draw(_backInputUIText);
	GetActor()->GetScene()->GetInputUI()->Draw(_tabChangeInputUI);;
	GetActor()->GetScene()->GetTextRenderer().Draw(_tabChangeInputUIText);
	GetActor()->GetScene()->GetInputUI()->Draw(_sortInputUI);;
	GetActor()->GetScene()->GetTextRenderer().Draw(_sortInputUIText);

	// テキスト描画
	GetActor()->GetScene()->GetTextRenderer().Draw(_itemName);
	GetActor()->GetScene()->GetTextRenderer().Draw(_itemDescription);
	GetActor()->GetScene()->GetTextRenderer().Draw(_itemQuantityText);
	GetActor()->GetScene()->GetTextRenderer().Draw(_pourchText);
	GetActor()->GetScene()->GetTextRenderer().Draw(_strageText);
}

// GUI描画
void ChestItemMenuController::DrawGui()
{
	if (ImGui::TreeNode(u8"ポーチ内アイテムトランスフォーム"))
	{
		_pourchItemTransform.DrawGui();
		ImGui::DragFloat2(u8"アイテムオフセット", &_pourchItemOffset.x, 1.0f);
		ImGui::DragFloat2(u8"アイテムスケール", &_pourchItemScale.x, 0.1f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"ストレージ内アイテムトランスフォーム"))
	{
		_strageItemTransform.DrawGui();
		ImGui::DragFloat2(u8"アイテムオフセット", &_strageItemOffset.x, 1.0f);
		ImGui::DragFloat2(u8"アイテムスケール", &_strageItemScale.x, 0.1f);
		ImGui::DragInt(u8"列数", &_strageItemColumnIndex);
		ImGui::DragInt(u8"行数", &_strageItemRowIndex);
		ImGui::DragInt(u8"ページ数", &_strageItemMaxPage);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"アイテム前面スプライト"))
	{
		_itemFrontSprite->DrawGui();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"アイテム背景スプライト"))
	{
		_itemBackSprite->DrawGui();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"プロパティスプライト"))
	{
		_propertySprite->DrawGui();
		ImGui::DragFloat2(u8"オフセット", &_propertyOffset.x, 1.0f);
		ImGui::DragFloat2(u8"スケール", &_propertyScale.x, 0.1f);
		ImGui::DragFloat(u8"画面外判定X座標", &_propertyInversPosisionX, 1.0f);
		ImGui::DragFloat(u8"テキスト間隔", &_propertyTextInterval, 1.0f);
		ImGui::DragFloat2(u8"テキスト原点", &_propertyTextOrigin.x, 0.1f);
		ImGui::DragFloat2(u8"テキストスケール", &_propertyTextScale.x, 0.1f);
		ImGui::ColorEdit4(u8"テキスト色", &_propertyTextColor.x);
		ImGui::ColorEdit4(u8"選択中テキスト色", &_propertySelectTextColor.x);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"個数表示用パラメータ"))
	{
		ImGui::DragFloat2(u8"オフセット", &_itemQuantityOffset.x, 1.0f);
		ImGui::DragFloat2(u8"スケール", &_itemQuantityScale.x, 0.1f);
		ImGui::TreePop();
	}
	_itemName.DrawGui(u8"名前");
	_itemDescription.DrawGui(u8"説明");
	_itemQuantityText.DrawGui(u8"個数");
	_pourchText.DrawGui(u8"ポーチテキスト");
	_strageText.DrawGui(u8"ストレージテキスト");
	if (ImGui::TreeNode(u8"入力UI"))
	{
		ImGui::DragFloat2(u8"選択入力UI位置", &_selectInputUI.position.x, 1.0f);
		ImGui::DragFloat2(u8"選択入力UIスケール", &_selectInputUI.scale.x, 0.1f);
		ImGui::ColorEdit4(u8"選択入力UI色", &_selectInputUI.color.x);
		_selectInputUIText.DrawGui(u8"選択入力UIText");

		ImGui::Separator();
		ImGui::DragFloat2(u8"戻る入力UI位置", &_backInputUI.position.x, 1.0f);
		ImGui::DragFloat2(u8"戻る入力UIスケール", &_backInputUI.scale.x, 0.1f);
		ImGui::ColorEdit4(u8"戻る入力UI色", &_backInputUI.color.x);
		_backInputUIText.DrawGui(u8"戻る入力UIText");

		ImGui::Separator();
		ImGui::DragFloat2(u8"タブ切り替えUI位置", &_tabChangeInputUI.position.x, 1.0f);
		ImGui::DragFloat2(u8"タブ切り替えUIスケール", &_tabChangeInputUI.scale.x, 0.1f);
		ImGui::ColorEdit4(u8"タブ切り替えUI色", &_tabChangeInputUI.color.x);
		_tabChangeInputUIText.DrawGui(u8"タブ切り替えUIText");

		ImGui::Separator();
		ImGui::DragFloat2(u8"ソートUI位置", &_sortInputUI.position.x, 1.0f);
		ImGui::DragFloat2(u8"ソートUIスケール", &_sortInputUI.scale.x, 0.1f);
		ImGui::ColorEdit4(u8"ソートUI色", &_sortInputUI.color.x);
		_sortInputUIText.DrawGui(u8"ソートUIText");

		ImGui::TreePop();
	}
}

// インデックス追加
void ChestItemMenuController::AddIndex(int direction)
{
	bool up = (direction & static_cast<unsigned int>(ChestUIController::InputDirection::Up)) != 0;
	bool down = (direction & static_cast<unsigned int>(ChestUIController::InputDirection::Down)) != 0;
	bool left = (direction & static_cast<unsigned int>(ChestUIController::InputDirection::Left)) != 0;
	bool right = (direction & static_cast<unsigned int>(ChestUIController::InputDirection::Right)) != 0;

	// アイテムのプロパティ表示中
	if (_state == State::PropertySelecting)
	{
		switch (_property)
		{
		case ChestItemMenuController::Property::MoveAll:
			if (up)
				_property = Property::Cancel;
			else if (down)
				_property = Property::MoveOne;
			break;
		case ChestItemMenuController::Property::MoveOne:
			if (up)
				_property = Property::MoveAll;
			else if (down)
				_property = Property::Cancel;
			break;
		case ChestItemMenuController::Property::Cancel:
			if (up)
				_property = Property::MoveOne;
			else if (down)
				_property = Property::MoveAll;
			break;
		}

		return;
	}

	switch (_tab)
	{
	case ChestItemMenuController::Tab::Pourch:
		// ストレージへ移動
		if (up || down)
		{
			_tab = Tab::Strage;
		}
		// 右端から左端へ移動
		else if (_currentIndex == UserDataManager::MaxPouchItemCount - 1 && right)
		{
			_currentIndex = 0;
		}
		// 左端から右端へ移動
		else if (_currentIndex == 0 && left)
		{
			_currentIndex = (UserDataManager::MaxPouchItemCount - 1);

		}
		else
		{
			if (left)
				_currentIndex--;
			else if (right)
				_currentIndex++;
		}
		break;
	case ChestItemMenuController::Tab::Strage:
		// 左端から右端へ移動
		if (_currentIndex % _strageItemColumnIndex == 0 && left)
		{
			_currentIndex += _strageItemColumnIndex - 1;
		}
		// 右端から左端へ移動
		else if (_currentIndex % _strageItemColumnIndex == _strageItemColumnIndex - 1 && right)
		{
			_currentIndex -= _strageItemColumnIndex - 1;
		}
		// 下端から上端へ移動
		else if (_currentIndex / _strageItemColumnIndex == (_strageItemRowIndex - 1) && down)
		{
			_currentIndex = _currentIndex % (_strageItemColumnIndex);
		}
		// 上端から下端へ移動
		else if (_currentIndex / _strageItemColumnIndex == 0 && up)
		{
			_currentIndex = (_strageItemRowIndex - 1) * _strageItemColumnIndex + (_currentIndex % _strageItemColumnIndex);
		}
		else
		{
			if (left)
				_currentIndex--;
			else if (right)
				_currentIndex++;
			if (up)
				_currentIndex -= _strageItemColumnIndex;
			else if (down)
				_currentIndex += _strageItemColumnIndex;
		}
		break;
	}
}

// 次へ進む
void ChestItemMenuController::NextState()
{
	auto spriteRenderer = _spriteRenderer.lock();
	if (!spriteRenderer)
		return;
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;

	switch (_state)
	{
	case ChestItemMenuController::State::ItemSelecting:
		// アイテムを選択しているか確認
		switch (_tab)
		{
		case ChestItemMenuController::Tab::Pourch:
		{
			auto pouchItem = &userDataManager->GetPouchItems()[_currentIndex];
			if (!pouchItem || pouchItem->itemIndex == -1)
				return;
		}
		break;
		case ChestItemMenuController::Tab::Strage:
		{
			auto iter = userDataManager->GetAcquiredItemDataMap().find(_currentIndex);
			if (iter == userDataManager->GetAcquiredItemDataMap().end())
				return;
		}
		break;
		}

		_state = State::PropertySelecting;
		// 選択中のアイテムの隣に表示
		{
			_propertySprite->GetRectTransform().SetLocalScale(_propertyScale);
			Vector2 localPosition;
			switch (_tab)
			{
			case ChestItemMenuController::Tab::Pourch:
				localPosition = _pourchItemTransform.GetLocalPosition() + _pourchItemOffset * static_cast<float>(_currentIndex);
				localPosition += _propertyOffset;
				_propertySprite->GetRectTransform().SetLocalPosition(localPosition);
				_propertySprite->UpdateTransform(&spriteRenderer->GetRectTransform(PourchBackSpr));
				break;
			case ChestItemMenuController::Tab::Strage:
				localPosition = _strageItemTransform.GetLocalPosition();
				localPosition += _propertyOffset;
				localPosition.x += (_currentIndex % _strageItemColumnIndex) * _strageItemOffset.x;
				localPosition.y += (_currentIndex / _strageItemColumnIndex) * _strageItemOffset.y;
				_propertySprite->GetRectTransform().SetLocalPosition(localPosition);
				_propertySprite->UpdateTransform(&spriteRenderer->GetRectTransform(StrageBackSpr));
				break;
			}

			// プロパティが画面外に出るなら位置を左に変更
			if (_propertySprite->GetRectTransform().GetWorldPosition().x > _propertyInversPosisionX)
			{
				Vector2 position = _propertySprite->GetRectTransform().GetWorldPosition();
				position.x -= _propertyOffset.x * 2.0f;
				position.x -= _propertySprite->GetTexSize().x * _propertySprite->GetRectTransform().GetLocalScale().x;
				_propertySprite->GetRectTransform().SetLocalPosition(position);
				_propertySprite->UpdateTransform();
			}
		}

		break;
	case ChestItemMenuController::State::PropertySelecting:
		switch (_property)
		{
		case ChestItemMenuController::Property::MoveAll:
			ItemMove(true);
			break;
		case ChestItemMenuController::Property::MoveOne:
			ItemMove(false);
			break;
		}
		PreviousState();
		break;
	}
}

// 前の状態へ戻る
// メニューを閉じる場合はtrueを返す
bool ChestItemMenuController::PreviousState()
{
	switch (_state)
	{
	case ChestItemMenuController::State::ItemSelecting:
		return true;
	case ChestItemMenuController::State::PropertySelecting:
		_state = State::ItemSelecting;
		break;
	}

	return false;
}

// リセット
void ChestItemMenuController::Reset()
{
	_tab = Tab::Pourch;
	_state = State::ItemSelecting;
	_currentIndex = 0;
}

// タブ切り替え
void ChestItemMenuController::ChangeTab()
{
	// アイテムのプロパティ表示中は操作不可
	if (_state == State::PropertySelecting)
		return;

	switch (_tab)
	{
	case ChestItemMenuController::Tab::Pourch:
		_tab = Tab::Strage;
		break;
	case ChestItemMenuController::Tab::Strage:
		_tab = Tab::Pourch;
		// ポーチの最大アイテム数を超えていたら調整
		if (_currentIndex >= UserDataManager::MaxPouchItemCount)
		{
			_currentIndex = UserDataManager::MaxPouchItemCount - 1;
		}
		break;
	}
}

// ポーチ整列
void ChestItemMenuController::SortPourch()
{
	// アイテムのプロパティ表示中は操作不可
	if (_state == State::PropertySelecting)
		return;

	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;

	userDataManager->SortPouchItems();
	// ポーチ内アイテムをアイテム番号順にソート
	for (int i = 0; i < UserDataManager::MaxPouchItemCount - 1; i++)
	{
		for (int j = i + 1; j < UserDataManager::MaxPouchItemCount; j++)
		{
			auto& itemA = userDataManager->GetPouchItems()[i];
			auto& itemB = userDataManager->GetPouchItems()[j];
			if (itemA.itemIndex == -1 || itemB.itemIndex == -1)
				continue;
			if (itemA.itemIndex > itemB.itemIndex)
			{
				UserDataManager::PouchItemData temp = itemA;
				itemA.itemIndex = itemB.itemIndex;
				itemA.quantity = itemB.quantity;
				itemB.itemIndex = temp.itemIndex;
				itemB.quantity = temp.quantity;
			}
		}
	}
}

#pragma region 入出力
// ファイル読み込み
bool ChestItemMenuController::LoadFromFile()
{
	std::string filePath = this->GetDirectory();
	filePath += this->GetName();
	filePath += ".json";

	nlohmann::json json;
	if (Exporter::LoadJsonFile(filePath, &json))
	{
		_pourchItemTransform	= json.value("PourchItemTransform", _pourchItemTransform);
		_pourchItemOffset		= json.value("PourchItemOffset", _pourchItemOffset);
		_pourchItemScale		= json.value("PourchItemScale", _pourchItemScale);

		_strageItemTransform	= json.value("StrageItemTransform", _strageItemTransform);
		_strageItemOffset		= json.value("StrageItemOffset", _strageItemOffset);
		_strageItemScale		= json.value("StrageItemScale", _strageItemScale);
		_strageItemColumnIndex	= json.value("StrageItemColumnIndex", 5);
		_strageItemRowIndex		= json.value("StrageItemRowIndex", 10);
		_strageItemMaxPage		= json.value("StrageItemMaxPage", 10);

		_propertyOffset			= json.value("PropertyOffset", _propertyOffset);
		_propertyScale			= json.value("PropertyScale", _propertyScale);
		_propertyInversPosisionX = json.value("PropertyInversPosisionX", _propertyInversPosisionX);
		_propertyTextInterval	= json.value("PropertyTextInterval", _propertyTextInterval);
		_propertyTextOrigin		= json.value("PropertyTextOrigin", _propertyTextOrigin);
		_propertyTextScale		= json.value("PropertyTextScale", _propertyTextScale);
		_propertyTextColor		= json.value("PropertyTextColor", _propertyTextColor);
		_propertySelectTextColor = json.value("PropertySelectTextColor", _propertySelectTextColor);

        _itemQuantityOffset		= json.value("ItemQuantityOffset", _itemQuantityOffset);
        _itemQuantityScale		= json.value("ItemQuantityScale", _itemQuantityScale);

		if (json.contains("Text"))
		{
			auto& sub = json["Text"];
			_itemName.position = sub.value("itemNamePosition", _itemName.position);
			_itemName.scale = sub.value("itemNameScale", _itemName.scale);
			_itemName.color = sub.value("itemNameColor", _itemName.color);
			_itemName.origin = sub.value("itemNameOrigin", _itemName.origin);

			_itemDescription.position = sub.value("itemDescriptionPosition", _itemDescription.position);
			_itemDescription.scale = sub.value("itemDescriptionScale", _itemDescription.scale);
			_itemDescription.color = sub.value("itemDescriptionColor", _itemDescription.color);
			_itemDescription.origin = sub.value("itemDescriptionOrigin", _itemDescription.origin);

			_itemQuantityText.position = sub.value("itemQuantityTextPosition", _itemQuantityText.position);
			_itemQuantityText.scale = sub.value("itemQuantityTextScale", _itemQuantityText.scale);
			_itemQuantityText.color = sub.value("itemQuantityTextColor", _itemQuantityText.color);
			_itemQuantityText.origin = sub.value("itemQuantityTextOrigin", _itemQuantityText.origin);

			_pourchText.position = sub.value("pourchTextPosition", _pourchText.position);
			_pourchText.scale = sub.value("pourchTextScale", _pourchText.scale);
			_pourchText.color = sub.value("pourchTextColor", _pourchText.color);
			_pourchText.origin = sub.value("pourchTextOrigin", _pourchText.origin);

			_strageText.position = sub.value("strageTextPosition", _strageText.position);
			_strageText.scale = sub.value("strageTextScale", _strageText.scale);
			_strageText.color = sub.value("strageTextColor", _strageText.color);
			_strageText.origin = sub.value("strageTextOrigin", _strageText.origin);
		}

		if (json.contains("InputUI"))
		{
			auto& sub = json["InputUI"];
			_selectInputUI.position = sub.value("selectInputUIPosition", _selectInputUI.position);
			_selectInputUI.scale = sub.value("selectInputUIScale", _selectInputUI.scale);
			_selectInputUI.color = sub.value("selectInputUIColor", _selectInputUI.color);
			{
				auto& ssub = sub["selectInputUI"];
				_selectInputUIText.position = ssub.value("Position", _selectInputUIText.position);
				_selectInputUIText.scale = ssub.value("Scale", _selectInputUIText.scale);
				_selectInputUIText.color = ssub.value("Color", _selectInputUIText.color);
				_selectInputUIText.origin = ssub.value("Origin", _selectInputUIText.origin);
			}
			_backInputUI.position = sub.value("backInputUIPosition", _backInputUI.position);
			_backInputUI.scale = sub.value("backInputUIScale", _backInputUI.scale);
			_backInputUI.color = sub.value("backInputUIColor", _backInputUI.color);
			{
				auto& ssub = sub["backInputUI"];
				_backInputUIText.position = ssub.value("Position", _backInputUIText.position);
				_backInputUIText.scale = ssub.value("Scale", _backInputUIText.scale);
				_backInputUIText.color = ssub.value("Color", _backInputUIText.color);
				_backInputUIText.origin = ssub.value("Origin", _backInputUIText.origin);
			}
			_tabChangeInputUI.position = sub.value("tabChangeInputUIPosition", _tabChangeInputUI.position);
			_tabChangeInputUI.scale = sub.value("tabChangeInputUIScale", _tabChangeInputUI.scale);
			_tabChangeInputUI.color = sub.value("tabChangeInputUIColor", _tabChangeInputUI.color);
			{
				auto& ssub = sub["tabChangeInputUI"];
				_tabChangeInputUIText.position = ssub.value("Position", _tabChangeInputUIText.position);
				_tabChangeInputUIText.scale = ssub.value("Scale", _tabChangeInputUIText.scale);
				_tabChangeInputUIText.color = ssub.value("Color", _tabChangeInputUIText.color);
				_tabChangeInputUIText.origin = ssub.value("Origin", _tabChangeInputUIText.origin);
			}
			_sortInputUI.position = sub.value("sortInputUIPosition", _sortInputUI.position);
			_sortInputUI.scale = sub.value("sortInputUIScale", _sortInputUI.scale);
			_sortInputUI.color = sub.value("sortInputUIColor", _sortInputUI.color);
			{
				auto& ssub = sub["sortInputUI"];
				_sortInputUIText.position = ssub.value("Position", _sortInputUIText.position);
				_sortInputUIText.scale = ssub.value("Scale", _sortInputUIText.scale);
				_sortInputUIText.color = ssub.value("Color", _sortInputUIText.color);
				_sortInputUIText.origin = ssub.value("Origin", _sortInputUIText.origin);
			}
		}

	}
	return false;
}

// ファイル保存
bool ChestItemMenuController::SaveToFile()
{
	std::string filePath = this->GetDirectory();
	filePath += this->GetName();
	filePath += ".json";

	nlohmann::json json;
	json["PourchItemTransform"] = _pourchItemTransform;
	json["PourchItemOffset"] = _pourchItemOffset;
	json["PourchItemScale"] = _pourchItemScale;

	json["StrageItemTransform"] = _strageItemTransform;
	json["StrageItemOffset"] = _strageItemOffset;
	json["StrageItemScale"] = _strageItemScale;
	json["StrageItemColumnIndex"] = _strageItemColumnIndex;
	json["StrageItemRowIndex"] = _strageItemRowIndex;
	json["StrageItemMaxPage"] = _strageItemMaxPage;

	json["PropertyOffset"] = _propertyOffset;
	json["PropertyScale"] = _propertyScale;
	json["PropertyInversPosisionX"] = _propertyInversPosisionX;
	json["PropertyTextInterval"] = _propertyTextInterval;
	json["PropertyTextOrigin"] = _propertyTextOrigin;
	json["PropertyTextScale"] = _propertyTextScale;
	json["PropertyTextColor"] = _propertyTextColor;
	json["PropertySelectTextColor"] = _propertySelectTextColor;

	json["ItemQuantityOffset"] = _itemQuantityOffset;
    json["ItemQuantityScale"] = _itemQuantityScale;

	{
		auto& sub = json["Text"];
		sub["itemNamePosition"] = _itemName.position;
		sub["itemNameScale"] = _itemName.scale;
		sub["itemNameColor"] = _itemName.color;
		sub["itemNameOrigin"] = _itemName.origin;

		sub["itemDescriptionPosition"] = _itemDescription.position;
		sub["itemDescriptionScale"] = _itemDescription.scale;
		sub["itemDescriptionColor"] = _itemDescription.color;
		sub["itemDescriptionOrigin"] = _itemDescription.origin;

		sub["itemQuantityTextPosition"] = _itemQuantityText.position;
		sub["itemQuantityTextScale"] = _itemQuantityText.scale;
		sub["itemQuantityTextColor"] = _itemQuantityText.color;
		sub["itemQuantityTextOrigin"] = _itemQuantityText.origin;

		sub["pourchTextPosition"] = _pourchText.position;
		sub["pourchTextScale"] = _pourchText.scale;
		sub["pourchTextColor"] = _pourchText.color;
		sub["pourchTextOrigin"] = _pourchText.origin;

		sub["strageTextPosition"] = _strageText.position;
		sub["strageTextScale"] = _strageText.scale;
		sub["strageTextColor"] = _strageText.color;
		sub["strageTextOrigin"] = _strageText.origin;
	}

	{
		auto& sub = json["InputUI"];
		sub["selectInputUIPosition"] = _selectInputUI.position;
		sub["selectInputUIScale"] = _selectInputUI.scale;
		sub["selectInputUIColor"] = _selectInputUI.color;
		{
			auto& ssub = sub["selectInputUI"];
			ssub["Position"] = _selectInputUIText.position;
			ssub["Scale"] = _selectInputUIText.scale;
			ssub["Color"] = _selectInputUIText.color;
			ssub["Origin"] = _selectInputUIText.origin;
		}

		sub["backInputUIPosition"] = _backInputUI.position;
		sub["backInputUIScale"] = _backInputUI.scale;
		sub["backInputUIColor"] = _backInputUI.color;
		{
			auto& ssub = sub["backInputUI"];
			ssub["Position"] = _backInputUIText.position;
			ssub["Scale"] = _backInputUIText.scale;
			ssub["Color"] = _backInputUIText.color;
			ssub["Origin"] = _backInputUIText.origin;
		}

		sub["tabChangeInputUIPosition"] = _tabChangeInputUI.position;
		sub["tabChangeInputUIScale"] = _tabChangeInputUI.scale;
		sub["tabChangeInputUIColor"] = _tabChangeInputUI.color;
		{
			auto& ssub = sub["tabChangeInputUI"];
			ssub["Position"] = _tabChangeInputUIText.position;
			ssub["Scale"] = _tabChangeInputUIText.scale;
			ssub["Color"] = _tabChangeInputUIText.color;
			ssub["Origin"] = _tabChangeInputUIText.origin;
		}

		sub["sortInputUIPosition"] = _sortInputUI.position;
		sub["sortInputUIScale"] = _sortInputUI.scale;
		sub["sortInputUIColor"] = _sortInputUI.color;
		{
			auto& ssub = sub["sortInputUI"];
			ssub["Position"] = _sortInputUIText.position;
			ssub["Scale"] = _sortInputUIText.scale;
			ssub["Color"] = _sortInputUIText.color;
			ssub["Origin"] = _sortInputUIText.origin;
		}

	}

	return Exporter::SaveJsonFile(filePath, json);
}
#pragma endregion

// ポーチメニュー描画
void ChestItemMenuController::RenderPourch(const RenderContext& rc)
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;
	auto spriteRenderer = _spriteRenderer.lock();
	if (!spriteRenderer)
		return;
	auto& textureRenderer = GetActor()->GetScene()->GetTextureRenderer();
	auto itemManager = ResourceManager::Instance().GetResourceAs<ItemManager>("ItemManager");
	if (!itemManager)
		return;
	auto iconCanvas = itemManager->GetItemIconCanvas();
	if (!iconCanvas)
		return;
	Vector2 canvasScale = {
		static_cast<float>(itemManager->GetItemIconCanvas()->GetCanvasScale().x),
		static_cast<float>(itemManager->GetItemIconCanvas()->GetCanvasScale().y)
	};

	float alpha = spriteRenderer->GetOverallAlpha();
	// 開いていないなら半透明
	if (_tab != Tab::Pourch)
	{
		alpha *= 0.5f;
	}

	// ポーチ内のアイテム描画
	auto PourchItemDraw = [&](int indexOffset)
		{
			int pouchItemIndex = indexOffset;
			Vector2 position = _pourchItemTransform.GetWorldPosition() +
				_pourchItemOffset * static_cast<float>(indexOffset);
			Vector2 scale = _pourchItemTransform.GetWorldScale();

			// 背景描画
			_itemBackSprite->Render(rc, textureRenderer, position, scale);

			auto pouchItem = &userDataManager->GetPouchItems()[pouchItemIndex];
			if (!pouchItem)
				return;

			auto itemData = userDataManager->GetAcquiredItemData(pouchItem->itemIndex);
			if (!itemData)
				return;
			auto baseData = itemData->GetBaseData();
			if (!baseData)
				return;

			auto texturedata = itemManager->GetItemIconTextureData(baseData->iconIndex);
			Vector4 color = baseData->color;
			color.w *= alpha;
			textureRenderer.Render(
				rc,
				iconCanvas->GetColorSRV().GetAddressOf(),
				canvasScale,
				position,
				_pourchItemScale,
				texturedata->texPosition,
				texturedata->texSize,
				Vector2::Zero,
				_pourchItemTransform.GetWorldAngle(),
				color);
			// オーバーレイアイコンの描画
			auto overlayIconTexture = itemManager->GetOverlayIconTexture();
			if (overlayIconTexture && baseData->overlayIconIndex >= 0)
			{
				auto texturedata = itemManager->GetItemIconTextureData(baseData->overlayIconIndex);
				color = Vector4::White;
				color.w *= alpha;
				GetActor()->GetScene()->GetTextureRenderer().Render(
					rc,
					overlayIconTexture,
					position,
					_pourchItemScale,
					texturedata->texPosition,
					texturedata->texSize,
					Vector2::Zero,
					_pourchItemTransform.GetWorldAngle(),
					color);
			}
			// 個数表示
			// プロパティ表示と重なっている場合は表示しない
			if (_state != State::PropertySelecting ||
				!_propertySprite->IsHit(position + _itemQuantityOffset))
			{
				bool isMax = baseData->maxCountInpouch == -1 ||
					pouchItem->quantity == baseData->maxCountInpouch;
				GetActor()->GetScene()->GetTextRenderer().Draw(
					FontType::MSGothic,
					pouchItem->quantity != -1 ? std::to_string(pouchItem->quantity).c_str() : "inf",
					position + _itemQuantityOffset,
					isMax ? Vector4::Red : Vector4::White,
					_pourchItemTransform.GetWorldAngle(),
					Vector2::Zero,
					_itemQuantityScale);
			}
		};
	// ポーチ内のアイテム描画
	for (int i = 0; i < UserDataManager::MaxPouchItemCount; ++i)
	{
		PourchItemDraw(i);
	}
}

// ストレージメニュー描画
void ChestItemMenuController::RenderStrage(const RenderContext& rc)
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;
	auto spriteRenderer = _spriteRenderer.lock();
	if (!spriteRenderer)
		return;
	auto& textureRenderer = GetActor()->GetScene()->GetTextureRenderer();
	auto itemManager = ResourceManager::Instance().GetResourceAs<ItemManager>("ItemManager");
	if (!itemManager)
		return;
	auto iconCanvas = itemManager->GetItemIconCanvas();
	if (!iconCanvas)
		return;
	Vector2 canvasScale = {
		static_cast<float>(itemManager->GetItemIconCanvas()->GetCanvasScale().x),
		static_cast<float>(itemManager->GetItemIconCanvas()->GetCanvasScale().y)
	};

	float alpha = spriteRenderer->GetOverallAlpha();
	// 開いていないなら半透明
	if (_tab != Tab::Strage)
	{
		alpha *= 0.5f;
	}

	int strageItemCount = _strageItemColumnIndex * _strageItemRowIndex;
	// ストレージの背景描画
	for (int index = 0; index < strageItemCount; ++index)
	{
		Vector2 position = _strageItemTransform.GetWorldPosition();
		position.x += (index % _strageItemColumnIndex) * _strageItemOffset.x;
		position.y += (index / _strageItemColumnIndex) * _strageItemOffset.y;
		Vector2 scale = _strageItemTransform.GetWorldScale();

		// 背景描画
		_itemBackSprite->Render(rc, textureRenderer, position, scale);
	}

	// ストレージ内のアイテム描画
	int drawIndex = 0;
	for (auto& [index, itemData] : userDataManager->GetAcquiredItemDataMap())
	{
		Vector2 position = _strageItemTransform.GetWorldPosition();
		position.x += (drawIndex % _strageItemColumnIndex) * _strageItemOffset.x;
		position.y += (drawIndex / _strageItemColumnIndex) * _strageItemOffset.y;

		auto baseData = itemData.GetBaseData();
		if (!baseData)
			continue;

		auto texturedata = itemManager->GetItemIconTextureData(baseData->iconIndex);
		Vector4 color = baseData->color;
		color.w *= alpha;
		textureRenderer.Render(
			rc,
			iconCanvas->GetColorSRV().GetAddressOf(),
			canvasScale,
			position,
			_strageItemScale,
			texturedata->texPosition,
			texturedata->texSize,
			Vector2::Zero,
			_pourchItemTransform.GetWorldAngle(),
			color);
		// オーバーレイアイコンの描画
		auto overlayIconTexture = itemManager->GetOverlayIconTexture();
		if (overlayIconTexture && baseData->overlayIconIndex >= 0)
		{
			auto texturedata = itemManager->GetItemIconTextureData(baseData->overlayIconIndex);
			color = Vector4::White;
			color.w *= alpha;
			GetActor()->GetScene()->GetTextureRenderer().Render(
				rc,
				overlayIconTexture,
				position,
				_pourchItemScale,
				texturedata->texPosition,
				texturedata->texSize,
				Vector2::Zero,
				_pourchItemTransform.GetWorldAngle(),
				color);
		}
		// 個数表示
		// プロパティ表示と重なっている場合は表示しない
		if (_state != State::PropertySelecting ||
			!_propertySprite->IsHit(position + _itemQuantityOffset))
		{
			auto pouchItems = userDataManager->GetPouchItemFromItemIndex(index);
			// ポーチにアイテムが含まれているなら所持数だけ数値を減らす
			int pouchQuantity = pouchItems ? pouchItems->quantity : 0;
			int quantity = itemData.quantity != -1 ?
				std::max<int>(0, itemData.quantity - pouchQuantity) : itemData.quantity;

			GetActor()->GetScene()->GetTextRenderer().Draw(
				FontType::MSGothic,
				itemData.quantity != -1 ? std::to_string(quantity).c_str() : "inf",
				position + _itemQuantityOffset,
				Vector4::White,
				_pourchItemTransform.GetWorldAngle(),
				Vector2::Zero,
				_itemQuantityScale);
		}

		drawIndex++;
	}
}

// アイテム移動処理
void ChestItemMenuController::ItemMove(bool all)
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;

	// 選択しているタブがポーチの時
	if (_tab == Tab::Pourch)
	{
		auto pouchItem = &userDataManager->GetPouchItems()[_currentIndex];
		if (!pouchItem)
			return;
		if (pouchItem->itemIndex == -1)
			return;

		// ポーチのアイテム所持数を減らす
		pouchItem->quantity -= all ? pouchItem->quantity : 1;

		// 所持数が0以下ならアイテムを削除
		if (pouchItem->quantity <= 0)
		{
			pouchItem->itemIndex = -1;
			pouchItem->quantity = 0;
			// 整理
			userDataManager->SortPouchItems();
		}

		return;
	}

	// 選択しているタブがストレージの時
	if (_tab == Tab::Strage)
	{
		auto iter = userDataManager->GetAcquiredItemDataMap().find(_currentIndex);
		if (iter == userDataManager->GetAcquiredItemDataMap().end())
			return;
		auto& itemData = iter->second;
		if (itemData.quantity <= 0)
			return;
		auto baseData = itemData.GetBaseData();
		if (!baseData)
			return;

		// 最後のポーチ番号を取得
		int lastPouchIndex = -1;
		for (int i = 0; i < UserDataManager::MaxPouchItemCount; ++i)
		{
			auto pouchItem = &userDataManager->GetPouchItems()[i];
			if (pouchItem->itemIndex == -1)
			{
				lastPouchIndex = i;
				break;
			}
			// 同じアイテムがあればそこに追加
			if (pouchItem->itemIndex == iter->first)
			{
				pouchItem->quantity += all ? itemData.quantity : 1;
				// 最大所持数制限
				pouchItem->quantity = baseData->maxCountInpouch != -1 ?
					std::min(pouchItem->quantity, baseData->maxCountInpouch) : pouchItem->quantity;

				// 整理
				userDataManager->SortPouchItems();
				return;
			}
		}
		// ポーチに空きがない場合は移動できない
		if (lastPouchIndex == -1)
			return;
		// ポーチにアイテムを追加
		auto pouchItem = &userDataManager->GetPouchItems()[lastPouchIndex];
		pouchItem->itemIndex = iter->first;
		pouchItem->quantity += all ? itemData.quantity : 1;
		// 最大所持数制限
		pouchItem->quantity = baseData->maxCountInpouch != -1 ?
			std::min(pouchItem->quantity, baseData->maxCountInpouch) : pouchItem->quantity;

		// 整理
		userDataManager->SortPouchItems();
	}
}
