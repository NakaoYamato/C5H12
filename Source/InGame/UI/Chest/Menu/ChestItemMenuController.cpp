#include "ChestItemMenuController.h"

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
			spriteRenderer->LoadTexture(BackSpr, L"");
			spriteRenderer->LoadTexture(PourchBackSpr, L"");
			spriteRenderer->LoadTexture(StrageBackSpr, L"");
			spriteRenderer->LoadTexture(FrontSpr, L"Data/Texture/UI/Chest/ItemFront.png");
			spriteRenderer->LoadTexture(BoxBackSpr, L"");
			spriteRenderer->LoadTexture(TextBoxSpr, L"");
		}
	}
}

// 更新処理
void ChestItemMenuController::Update(float elapsedTime)
{
}

// 3D描画後の描画処理
void ChestItemMenuController::DelayedRender(const RenderContext& rc)
{
}

// GUI描画
void ChestItemMenuController::DrawGui()
{

}

// インデックス追加
void ChestItemMenuController::AddIndex(int val)
{
}

// 次へ進む
void ChestItemMenuController::NextState()
{
}

// 前の状態へ戻る
// メニューを閉じる場合はtrueを返す
bool ChestItemMenuController::PreviousState()
{
	return true;
}

// リセット
void ChestItemMenuController::Reset()
{
	_state = State::Pourch;
}
