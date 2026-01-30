#include "QuestStartUIController.h"

#include "../../Library/Actor/UI/UIActor.h"

#include <imgui.h>

// 開始処理
void QuestStartUIController::Start()
{
    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();

    if (auto spriteRenderer = _spriteRenderer.lock())
    {
        if (!spriteRenderer->IsLoaded())
        {
            spriteRenderer->LoadTexture(FrontSpr, L"Data/Texture/UI/Quest/Start.png");
        }

        // 初期位置設定
		spriteRenderer->GetRectTransform(FrontSpr).SetLocalPosition(Vector2(1920.0f / 2.0f, 200.0f));
		spriteRenderer->GetRectTransform(FrontSpr).SetLocalScale(_entryScale);
		spriteRenderer->SetColorAlpha(FrontSpr, 0.0f);
        _sprMaterial = &spriteRenderer->GetSpriteData(FrontSpr).GetMaterial();
		_sprMaterial->SetShaderName("Dissolve");
        _sprMaterial->SetParameterMap(
            ResourceManager::Instance().GetResourceAs<SpriteShaderResource>()->GetShaderParameterKey("Dissolve"));
    }
}

// 更新処理
void QuestStartUIController::Update(float elapsedTime)
{
	auto spriteRenderer = _spriteRenderer.lock();
	if (!spriteRenderer)
        return;

	_timer += elapsedTime;

    float rate = 0.0f;
    switch (_state)
    {
    case QuestStartUIController::Entry:
        rate = MathF::Clamp01(_timer / _entryTime);
        spriteRenderer->GetRectTransform(FrontSpr).SetLocalScale(Vector2::Lerp(_entryScale, Vector2::One, rate, Easings::InCirc));
        spriteRenderer->SetColorAlpha(FrontSpr, rate);
        if (_timer >= _entryTime)
        {
            // フェードイン完了
            _state = QuestStartUIController::Loop;
            _timer = 0.0f;
            spriteRenderer->SetColorAlpha(FrontSpr, 1.0f);
        }
        break;
    case QuestStartUIController::Loop:
        if (_timer >= _LoopTime)
        {
            _state = QuestStartUIController::Exit;
            _timer = 0.0f;
        }
        break;
    case QuestStartUIController::Exit:
        rate = MathF::Clamp01(_timer / _exitTime);
        if (_sprMaterial)
            _sprMaterial->SetParameter("amount", rate);
        if (_timer >= _exitTime)
        {
            GetActor()->Remove();
        }
        break;
    }
}

// GUI描画
void QuestStartUIController::DrawGui()
{
}
