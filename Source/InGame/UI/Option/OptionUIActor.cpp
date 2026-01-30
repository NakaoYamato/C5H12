#include "OptionUIActor.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"

bool TestFlag = false;

// 生成時処理
void OptionUIActor::OnCreate()
{
	MenuUIActor::OnCreate();

	GetRectTransform().SetLocalPosition(Vector2(50.0f, 200.0f));

	// コールバック登録
#pragma region 選択時コールバック
	RegisterOptionSelectedCallback("PopPage", [this](MenuUIActor* owner) -> void
		{
			owner->PopPage();
		});
	RegisterOptionSelectedCallback("EndGame", [this](MenuUIActor* owner) -> void
		{
			PostMessage(Graphics::Instance().GetHwnd(), WM_CLOSE, 0, 0);
		});
	RegisterOptionSelectedCallback("EndAndSaveGame", [this](MenuUIActor* owner) -> void
		{
			// TODO セーブ処理

			PostMessage(Graphics::Instance().GetHwnd(), WM_CLOSE, 0, 0);
		});
	RegisterOptionSelectedCallback("SaveGame", [this](MenuUIActor* owner) -> void
		{
			// TODO セーブ処理

		});
	RegisterOptionSelectedCallback("CameraMoveX", [this](MenuUIActor* owner) -> void
		{
			if (auto mainCamera = GetScene()->GetMainCameraActor())
				mainCamera->SetInvertX(!mainCamera->IsInvertX());
		});
	RegisterOptionSelectedCallback("CameraMoveY", [this](MenuUIActor* owner) -> void
		{
			if (auto mainCamera = GetScene()->GetMainCameraActor())
				mainCamera->SetInvertY(!mainCamera->IsInvertY());
		});
#pragma endregion
#pragma region 選択可能かのコールバック
	RegisterCanSelectOptionCallback("CameraMoveX", [this](MenuUIActor* owner) -> bool
		{
			if (auto mainCamera = GetScene()->GetMainCameraActor())
				return mainCamera->IsInvertX();
			return false;
		});
	RegisterCanSelectOptionCallback("CameraMoveY", [this](MenuUIActor* owner) -> bool
		{
			if (auto mainCamera = GetScene()->GetMainCameraActor())
				return mainCamera->IsInvertY();
			return false;
		});
#pragma endregion


	// ウィジェット登録
	RegisterWidgetCreateFunc<PlayerEquipmentMenuWidget>();
	RegisterWidgetCreateFunc<PlayerStatusMenuWidget>();
	RegisterWidgetCreateFunc<GameSettingsMenuWidget>();

	SetFilePath("./Data/Resource/Actor/OptionUIActor/MenuUI.json");
	LoadFromFile();
}

// 起動フラグが変化したときの処理
void OptionUIActor::OnChangedActive(bool isActive)
{
	MenuUIActor::OnChangedActive(isActive);

	if (isActive)
	{
		PushPage("SelectMenu");
	}
	else
	{
		// すべてのページを破棄して戻る
		PopAllPages();
	}
}
