#include "LoadingSprController.h"

#include "../../Library/Algorithm/Converter.h"

#include <imgui.h>

// 開始処理
void LoadingSprController::Start()
{
	// 親シーン取得
	auto scene = GetActor()->GetScene();
	_loadingScene = dynamic_cast<SceneLoading*>(scene);

	_spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();
	if (auto spriteRenderer = _spriteRenderer.lock())
	{
		spriteRenderer->LoadTexture(BackSpr, L"Data/Texture/Loading/Back.jpg");
		spriteRenderer->GetRectTransform(BackSpr).SetLocalPosition(Vector2(960.0f, 540.0f));
		spriteRenderer->GetRectTransform(BackSpr).SetLocalScale(Vector2(0.4f, 0.4f));

		// 進捗バー背景の生成
		spriteRenderer->LoadTexture(LoadingBarBackSpr, L"Data/Texture/Loading/LoadingBar.png", Sprite::CenterAlignment::LeftCenter);
		spriteRenderer->GetRectTransform(LoadingBarBackSpr).SetLocalPosition(Vector2(1070.0f, 970.0f));
		spriteRenderer->GetRectTransform(LoadingBarBackSpr).SetLocalScale(Vector2(1.5f, 1.0f));
		spriteRenderer->SetColor(LoadingBarBackSpr, Vector4::Black);

		// 進捗バーの生成
		spriteRenderer->LoadTexture(LoadingBarSpr, L"Data/Texture/Loading/LoadingBar.png", Sprite::CenterAlignment::LeftCenter);
		spriteRenderer->GetRectTransform(LoadingBarSpr).SetLocalPosition(Vector2(1070.0f, 970.0f));
		spriteRenderer->GetRectTransform(LoadingBarSpr).SetLocalScale(Vector2(0.0f, 1.0f));

		// テキストスプライトの生成
		spriteRenderer->LoadTexture(TextSprs[0], L"Data/Texture/Loading/N.png");
		spriteRenderer->LoadTexture(TextSprs[1], L"Data/Texture/Loading/O.png");
		spriteRenderer->LoadTexture(TextSprs[2], L"Data/Texture/Loading/W.png");
		spriteRenderer->LoadTexture(TextSprs[3], L"");
		spriteRenderer->LoadTexture(TextSprs[4], L"Data/Texture/Loading/L.png");
		spriteRenderer->LoadTexture(TextSprs[5], L"Data/Texture/Loading/O.png");
		spriteRenderer->LoadTexture(TextSprs[6], L"Data/Texture/Loading/A.png");
		spriteRenderer->LoadTexture(TextSprs[7], L"Data/Texture/Loading/D.png");
		spriteRenderer->LoadTexture(TextSprs[8], L"Data/Texture/Loading/I.png");
		spriteRenderer->LoadTexture(TextSprs[9], L"Data/Texture/Loading/N.png");
		spriteRenderer->LoadTexture(TextSprs[10], L"Data/Texture/Loading/G.png");
		for (int i = 0; i < 11; ++i)
		{
			spriteRenderer->GetRectTransform(TextSprs[i]).SetLocalPosition(
				Vector2(_textStartPos.x + i * _textInterval, _textStartPos.y));
			spriteRenderer->GetRectTransform(TextSprs[i]).SetLocalScale(Vector2(0.4f, 0.4f));
		}
		spriteRenderer->GetRectTransform(TextSprs[3]).SetLocalScale(Vector2(0.0f, 0.0f));
	}

	_timer = 0.0f;
	_intervalTimer = 0.0f;
}

// 更新処理
void LoadingSprController::Update(float elapsedTime)
{
	auto spriteRenderer = _spriteRenderer.lock();
	if (!spriteRenderer)
		return;

	// 進捗バー
	float maxScale = spriteRenderer->GetRectTransform(LoadingBarBackSpr).GetLocalScale().x;
	float scale = EasingLerp(spriteRenderer->GetRectTransform(LoadingBarSpr).GetLocalScale().x,
		std::clamp(_loadingScene->GetCompletionLoading(), 0.0f, 1.0f) * maxScale,
		_loadingBarSpeed * elapsedTime);
    scale = std::clamp(scale, 0.0f, maxScale);
	spriteRenderer->GetRectTransform(LoadingBarSpr).SetLocalScale(Vector2(scale, 1.0f));

	// フェーズ処理
	if (_loadingScene && _loadingScene->IsNextSceneReady() && scale >= maxScale)
	{
		if (!_loadingScene->GetFade()->IsFading())
		{
			switch (_loadingScene->GetFade()->GetType())
			{
			case Fade::Type::FadeOut:
                _loadingScene->ChangeNextScene();
                break;
			default:
				_loadingScene->GetFade()->Start(Fade::Type::FadeOut, 1.0f);
				break;
			}
		}
	}

	if (_intervalTimer > 0.0f)
	{
		_intervalTimer -= elapsedTime;
		if (_intervalTimer < 0.0f)
			_intervalTimer = 0.0f;
		for (int i = 0; i < 11; ++i)
		{
			spriteRenderer->GetRectTransform(TextSprs[i]).SetLocalPosition(
				Vector2(_textStartPos.x + i * _textInterval, _textStartPos.y));
		}
		return;
	}

	_timer += elapsedTime;
	for (int i = 0; i < 11; ++i)
	{
		float t = (_timer - i * _textMoveTime) / _textMoveTime;
		t += _textMoveDelay * i; // 少し早めに開始
		if (t < 0.0f || 1.0f < t)
		{
			spriteRenderer->GetRectTransform(TextSprs[i]).SetLocalPosition(
				Vector2(_textStartPos.x + i * _textInterval, _textStartPos.y));
			continue;
		}
		Vector2 offset{};
		offset.x = -std::sinf(t * 2.0f * DirectX::XM_PI) * _textMoveOffset.x;
		offset.y = -std::sinf(t * DirectX::XM_PI) * _textMoveOffset.y;
		spriteRenderer->GetRectTransform(TextSprs[i]).SetLocalPosition(
			Vector2(_textStartPos.x + i * _textInterval, _textStartPos.y) + offset);
	}
	if (_timer >= _textMoveTime * 10.0f)
	{
		_timer = 0.0f;
		_intervalTimer = _textMoveInterval;
	}
}

// GUI描画
void LoadingSprController::DrawGui()
{
	ImGui::DragFloat2(u8"テキスト開始位置", &_textStartPos.x, 1.0f);
	ImGui::DragFloat(u8"テキスト間隔", &_textInterval, 1.0f, 0.0f);
	ImGui::DragFloat(u8"テキスト移動時間", &_textMoveTime, 0.01f, 0.0f);
	ImGui::DragFloat(u8"テキスト移動間隔", &_textMoveInterval, 0.01f, 0.0f);
	ImGui::DragFloat2(u8"テキスト移動オフセット", &_textMoveOffset.x, 1.0f);
    ImGui::DragFloat(u8"テキスト移動遅延", &_textMoveDelay, 0.01f, 0.0f);
}
