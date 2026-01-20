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
		if (!spriteRenderer->IsLoaded())
		{
			spriteRenderer->LoadTexture(BackSpr, L"Data/Texture/Loading/Back.jpg");
			// 進捗バー背景の生成
			spriteRenderer->LoadTexture(LoadingBarBackSpr, L"Data/Texture/Loading/LoadingBar.png", Sprite::CenterAlignment::LeftCenter);
			// 進捗バーの生成
			spriteRenderer->LoadTexture(LoadingBarSpr, L"Data/Texture/Loading/LoadingBar.png", Sprite::CenterAlignment::LeftCenter);
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
		}
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
	float rate = _loadingScene->GetCompletionLoading();
	_loadingBarRate = EasingLerp(_loadingBarRate,
		rate,
		elapsedTime * _loadingBarSpeed);
	_loadingBarRate = MathF::Clamp01(_loadingBarRate);
	float scale = EasingLerp(0.0f, maxScale, _loadingBarRate);
    scale = MathF::Clamp(scale, 0.0f, maxScale);
	spriteRenderer->GetRectTransform(LoadingBarSpr).SetLocalScale(Vector2(scale, 1.0f));

	// フェーズ処理
	if (_loadingScene && _loadingScene->IsNextSceneReady())
	{
        _subTimer += elapsedTime;

		if (_loadingBarRate >= 1.0f - FLT_EPSILON || _subTimer >= 1.0f)
		{
			if (!_loadingScene->GetFade()->IsFading())
			{
				switch (_loadingScene->GetFade()->GetType())
				{
				case Fade::Type::FadeOut:
					_loadingScene->ChangeNextScene();
					// フェード開始
					_loadingScene->GetFade()->Start(Fade::Type::FadeIn, _fadeInTime);
					break;
				default:
					_loadingScene->GetFade()->Start(Fade::Type::FadeOut, _fadeOutTime);
					break;
				}
			}
		}
	}

	// 待機時間
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

	// テキスト移動
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
	// リセット
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
