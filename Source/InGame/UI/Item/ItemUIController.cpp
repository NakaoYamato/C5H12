#include "ItemUIController.h"

#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/Algorithm/Converter.h"

#include <imgui.h>
// 開始処理
void ItemUIController::Start()
{
	// 子供取得
	for (auto& child : GetActor()->GetChildren())
	{
		_sliderSpriteRenderer = child->GetComponent<SpriteRenderer>();
		break;
	}

    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();

    if (auto spriteRenderer = _spriteRenderer.lock())
    {
        spriteRenderer->LoadTexture(BackSpr, L"Data/Texture/UI/Item/ItemBack.png");
		spriteRenderer->GetRectTransform(BackSpr).SetLocalScale(Vector2(0.5f, 0.5f));
		spriteRenderer->SetColorAlpha(BackSpr, 0.5f);
    }

	_iconTransform.SetLocalScale(Vector2(0.8f, 0.8f));
	_quantityTransform.SetLocalPosition(Vector2(15.0f, 12.0f));

	_userDataManager = ResourceManager::Instance().GetResourceAs<UserDataManager>("UserDataManager");
	auto uiActor = std::dynamic_pointer_cast<UIActor>(GetActor());
	if (uiActor)
	{
		_myRectTransform = &uiActor->GetRectTransform();
	}

	// 初期状態は閉じている
	Close();
}

// 更新処理
void ItemUIController::Update(float elapsedTime)
{
	if (_myRectTransform)
	{
		switch (_state)
		{
		case ItemUIController::Closed:
			// UI表示
			GetActor()->GetScene()->GetInputUI()->Draw("Use", _INPUT_PRESSED("Use"), _itemUseUIPos, _itemUseUIScale, _itemUseUIColor);
			GetActor()->GetScene()->GetInputUI()->Draw("ItemSelect", _INPUT_PRESSED("ItemSelect"), _itemSelectUIPos, _itemSelectUIScale, _itemSelectUIColor);
			break;
		case ItemUIController::Opening:
			_stateTimer = _stateTimer + _lerpSpeed * elapsedTime;
			if (_stateTimer >= 1.0f)
			{
				_state = State::Opened;
				_stateTimer = 1.0f;
			}

			_myRectTransform->SetLocalPosition(
				Vector2::Lerp(_closePosition, _openPosition, _stateTimer));
			_myRectTransform->SetLocalScale(
				Vector2::Lerp(_closeScale, _openScale, _stateTimer));
			_iconTransform.SetLocalPosition(
				Vector2::Lerp(_closeItemPosition, _openItemPosition, _stateTimer));
			break;
		case ItemUIController::Opened:
			// UI表示
			if (Input::Instance().GetCurrentInputDevice() == Input::InputType::XboxPad)
			{
				GetActor()->GetScene()->GetInputUI()->Draw("ItemPrevSlide", _INPUT_PRESSED("ItemPrevSlide"), _itemSliderLUIPos, _itemSliderLUIScale, _itemSliderLUIColor);
				GetActor()->GetScene()->GetInputUI()->Draw("ItemNextSlide", _INPUT_PRESSED("ItemNextSlide"), _itemSliderRUIPos, _itemSliderRUIScale, _itemSliderRUIColor);
			}
			else
			{
				// マウスホイールの上下を表示
				GetActor()->GetScene()->GetInputUI()->DrawValue("MouseOldWheel", 0.0f, _itemSliderLUIPos, _itemSliderLUIScale, _itemSliderLUIColor);
				GetActor()->GetScene()->GetInputUI()->DrawValue("MouseOldWheel", 1.0f, _itemSliderRUIPos, _itemSliderRUIScale, _itemSliderRUIColor);
            }
			break;
		case ItemUIController::Closing:
			_stateTimer = std::clamp(_stateTimer + _lerpSpeed * elapsedTime, 0.0f, 1.0f);
			if (_stateTimer >= 1.0f)
			{
				_state = State::Closed;
				_stateTimer = 1.0f;
			}

			_myRectTransform->SetLocalPosition(
				Vector2::Lerp(_openPosition, _closePosition, _stateTimer));
			_myRectTransform->SetLocalScale(
				Vector2::Lerp(_openScale, _closeScale, _stateTimer));
			_iconTransform.SetLocalPosition(
				Vector2::Lerp(_openItemPosition, _closeItemPosition, _stateTimer));
			break;
		}
		_iconTransform.UpdateTransform(_myRectTransform);
		_quantityTransform.UpdateTransform(_myRectTransform);

	}
}

// 3D描画後の描画処理
void ItemUIController::DelayedRender(const RenderContext& rc)
{
	auto itemManager = ResourceManager::Instance().GetResourceAs<ItemManager>("ItemManager");
	if (!itemManager)
		return;
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;

	Vector2 canvasScale = {
		static_cast<float>(itemManager->GetItemIconCanvas()->GetCanvasScale().x),
		static_cast<float>(itemManager->GetItemIconCanvas()->GetCanvasScale().y)
	};
    // アイテムの描画
	auto ItemDraw = [&](int offset, float alpha, bool drawQuantity)
		{
			auto currentItem = userDataManager->GetPouchItem(_currentIndex + offset);
			if (currentItem->itemIndex < 0)
				return;
			auto baseData = itemManager->GetItemData(currentItem->itemIndex);
			if (!baseData)
				return;
			auto iconData = itemManager->GetItemIconTextureData(baseData->iconIndex);
			if (!iconData)
				return;

			Vector2 position = _iconTransform.GetWorldPosition();
			Vector2 scale = _iconTransform.GetWorldScale();
			if (offset != 0)
			{
				position.x += offset * _iconOffset.x;
				position.y += _iconOffset.y;

				scale.x *= _iconScaleOffset.x;
				scale.y *= _iconScaleOffset.y;
			}
			Vector2 iconCenter = {
				iconData->texSize.x / 2.0f,
				iconData->texSize.y
			};
			Vector4 color = baseData->color;
			color.w *= alpha;
			GetActor()->GetScene()->GetTextureRenderer().Render(
				rc,
				itemManager->GetItemIconCanvas()->GetColorSRV().GetAddressOf(),
				canvasScale,
				position,
				scale,
				iconData->texPosition,
				iconData->texSize,
				iconCenter,
				_iconTransform.GetWorldAngle(),
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
					scale,
					texturedata->texPosition,
					texturedata->texSize,
					iconCenter,
					_iconTransform.GetWorldAngle(),
					color);
			}

			// 所持数の描画
			if (drawQuantity)
			{
				bool isMax = baseData->maxCountInpouch == -1 ||
					currentItem->quantity == baseData->maxCountInpouch;
				GetActor()->GetScene()->GetTextRenderer().Draw(
					FontType::MSGothic,
					currentItem->quantity != -1 ? std::to_string(currentItem->quantity).c_str() : "inf",
					_quantityTransform.GetWorldPosition(),
					isMax ? _quantityMaxColor : _quantityColor,
					_quantityTransform.GetWorldAngle(),
					Vector2::Zero,
					_quantityTransform.GetWorldScale());

				// アイテム名描画
				std::wstring str = ToUtf16(baseData->name);
				Vector2 scale = _quantityTransform.GetWorldScale();
				scale.x *= _itemNameScaleOffset.x;
				scale.y *= _itemNameScaleOffset.y;
				GetActor()->GetScene()->GetTextRenderer().Draw(
					FontType::MSGothic,
					str.c_str(),
					_quantityTransform.GetWorldPosition() + _itemNameOffset,
					Vector4::Black,
					_quantityTransform.GetWorldAngle(),
					_itemNameOrigin,
					scale);
			}
		};

	ItemDraw(0, 1.0f, true);

	// ポーチ内のアイテム描画
	bool subItemAlpha = _sliderSpriteRenderer.lock()->GetOverallAlpha();
	int pouchItemCount = userDataManager->GetLastPouchItem()->pouchIndex;
	ItemDraw(+2, subItemAlpha, false);
	ItemDraw(+1, subItemAlpha, false);
	ItemDraw(-2, subItemAlpha, false);
	ItemDraw(-1, subItemAlpha, false);
}

// GUI描画
void ItemUIController::DrawGui()
{
	ImGui::DragFloat2(u8"開閉速度", &_lerpSpeed, 0.1f);
	if (ImGui::TreeNode(u8"アイコン"))
	{
		_iconTransform.DrawGui();
		ImGui::DragFloat2(u8"オフセット", &_iconOffset.x, 1.0f);
		ImGui::DragFloat2(u8"オフセットスケール", &_iconScaleOffset.x, 0.01f, 0.1f, 2.0f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode(u8"所持数テキスト"))
	{
		_quantityTransform.DrawGui();
		ImGui::ColorEdit4(u8"所持数色", &_quantityColor.x);
		ImGui::ColorEdit4(u8"所持数最大色", &_quantityMaxColor.x);
		ImGui::DragFloat2(u8"アイテム名オフセット", &_itemNameOffset.x, 1.0f);
		ImGui::DragFloat2(u8"アイテム名原点", &_itemNameOrigin.x, 0.1f);
		ImGui::DragFloat2(u8"アイテム名スケールオフセット", &_itemNameScaleOffset.x, 0.01f, 0.1f, 2.0f);
		ImGui::TreePop();
	}
	ImGui::Separator();

	ImGui::DragFloat(u8"ステートタイマー", &_stateTimer, 0.01f, 0.0f, 1.0f);
	ImGui::Separator();

	if (ImGui::TreeNode(u8"入力UI"))
	{
		ImGui::DragFloat2(u8"アイテム使用UI位置", &_itemUseUIPos.x, 1.0f);
		ImGui::DragFloat2(u8"アイテム使用UIスケール", &_itemUseUIScale.x, 0.01f, 0.1f, 3.0f);
		ImGui::ColorEdit4(u8"アイテム使用UI色", &_itemUseUIColor.x);
		ImGui::DragFloat2(u8"アイテム選択UI位置", &_itemSelectUIPos.x, 1.0f);
		ImGui::DragFloat2(u8"アイテム選択UIスケール", &_itemSelectUIScale.x, 0.01f, 0.1f, 3.0f);
		ImGui::ColorEdit4(u8"アイテム選択UI色", &_itemSelectUIColor.x);
		ImGui::DragFloat2(u8"スライダー左UI位置", &_itemSliderLUIPos.x, 1.0f);
		ImGui::DragFloat2(u8"スライダー左UIスケール", &_itemSliderLUIScale.x, 0.01f, 0.1f, 3.0f);
		ImGui::ColorEdit4(u8"スライダー左UI色", &_itemSliderLUIColor.x);
		ImGui::DragFloat2(u8"スライダー右UI位置", &_itemSliderRUIPos.x, 1.0f);
		ImGui::DragFloat2(u8"スライダー右UIスケール", &_itemSliderRUIScale.x, 0.01f, 0.1f, 3.0f);
		ImGui::ColorEdit4(u8"スライダー右UI色", &_itemSliderRUIColor.x);

		ImGui::TreePop();
	}

}

// 開く
void ItemUIController::Open()
{
	_state = State::Opening;
	_sliderSpriteRenderer.lock()->GetActor()->SetIsActive(true);
	_stateTimer = 0.0f;
}

// 閉じる
void ItemUIController::Close()
{
	_state = State::Closing;
	_sliderSpriteRenderer.lock()->GetActor()->SetIsActive(false);
	_stateTimer = 0.0f;
}
