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
		if (!spriteRenderer->LoadFromFile())
		{
			spriteRenderer->LoadTexture(BackSpr, L"");
			spriteRenderer->LoadTexture(PourchBackSpr, L"");
			spriteRenderer->LoadTexture(StrageBackSpr, L"");
			spriteRenderer->LoadTexture(FrontSpr, L"Data/Texture/UI/Chest/ItemFront.png");
			spriteRenderer->LoadTexture(BoxBackSpr, L"");
			spriteRenderer->LoadTexture(TextBoxSpr, L"");

			spriteRenderer->SetCenterAlignment(BackSpr, Sprite::LeftUp);
			spriteRenderer->GetRectTransform(BackSpr).SetLocalPosition(Vector2(80.0f, 380.0f));
			spriteRenderer->GetRectTransform(BackSpr).SetLocalScale(Vector2(110.0f, 40.0f));
			spriteRenderer->SetColor(BackSpr, Vector4(0.0f, 0.0f, 0.0f, 90.0f / 255.0f));

			spriteRenderer->SetCenterAlignment(PourchBackSpr, Sprite::LeftUp);
			spriteRenderer->SetParentName(PourchBackSpr, BackSpr);
			spriteRenderer->GetRectTransform(PourchBackSpr).SetReflectParentScale(false);

			spriteRenderer->SetCenterAlignment(StrageBackSpr, Sprite::LeftUp);
			spriteRenderer->SetParentName(StrageBackSpr, BackSpr);
			spriteRenderer->GetRectTransform(StrageBackSpr).SetReflectParentScale(false);

			spriteRenderer->SetCenterAlignment(BoxBackSpr, Sprite::LeftUp);
			spriteRenderer->GetRectTransform(BoxBackSpr).SetLocalPosition(Vector2(245.0f, 120.0f));
			spriteRenderer->GetRectTransform(BoxBackSpr).SetLocalScale(Vector2(34.5f, 49.0f));
			spriteRenderer->SetColor(BoxBackSpr, Vector4(0.0f, 0.0f, 0.0f, 0.0f));

			spriteRenderer->SetCenterAlignment(TextBoxSpr, Sprite::LeftUp);
			spriteRenderer->GetRectTransform(TextBoxSpr).SetLocalPosition(Vector2(810.0f, 110.0f));
			spriteRenderer->GetRectTransform(TextBoxSpr).SetLocalScale(Vector2(31.0f, 50.0f));
			spriteRenderer->SetColor(TextBoxSpr, Vector4(0.0f, 0.0f, 0.0f, 90.0f / 255.0f));
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
