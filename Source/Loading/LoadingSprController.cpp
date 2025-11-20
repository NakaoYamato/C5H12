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
			spriteRenderer->GetRectTransform(TextSprs[i]).SetLocalScale(Vector2(0.6f, 0.6f));
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

	if (_intervalTimer > 0.0f)
	{
		_intervalTimer -= elapsedTime;
		if (_intervalTimer < 0.0f)
			_intervalTimer = 0.0f;
		for (int i = 0; i < 11; ++i)
		{
			spriteRenderer->GetRectTransform(TextSprs[i]).SetLocalPosition(
				Vector2(_textStartPos.x + i * _textInterval, _textStartPos.y));
			spriteRenderer->GetRectTransform(TextSprs[i]).SetLocalScale(Vector2(0.6f, 0.6f));
		}
		return;
	}

	_timer += elapsedTime;
	for (int i = 0; i < 11; ++i)
	{
		float t = (_timer - i * _textMoveTime) / _textMoveTime;
		t -= 0.1f * i; // 少し早めに開始
		if (t < 0.0f || 1.0f < t)
		{
			spriteRenderer->GetRectTransform(TextSprs[i]).SetLocalPosition(
				Vector2(_textStartPos.x + i * _textInterval, _textStartPos.y));
			spriteRenderer->GetRectTransform(TextSprs[i]).SetLocalScale(Vector2(0.6f, 0.6f));
			continue;
		}
		Vector2 offset{};
		offset.x = -std::sinf(t * 2.0f * DirectX::XM_PI) * _textMoveOffset.x;
		offset.y = -std::sinf(t * DirectX::XM_PI) * _textMoveOffset.y;
		spriteRenderer->GetRectTransform(TextSprs[i]).SetLocalPosition(
			Vector2(_textStartPos.x + i * _textInterval, _textStartPos.y) + offset);
		spriteRenderer->GetRectTransform(TextSprs[i]).SetLocalScale(Vector2(0.6f, 0.6f));
	}
	if (_timer >= _textMoveTime * 12.0f)
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
}
