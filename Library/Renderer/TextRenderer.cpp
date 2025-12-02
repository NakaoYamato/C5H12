#include "TextRenderer.h"

#include "../../Algorithm/Converter.h"

#include <imgui.h>

/// 初期化処理
void TextRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	_spriteBatch = std::make_unique<DirectX::SpriteBatch>(dc);
	_spriteFonts[static_cast<int>(FontType::MSGothic)] = std::make_unique<DirectX::SpriteFont>(device, L"./Data/Font/MSGothic.spritefont");

	_fontScale[static_cast<int>(FontType::MSGothic)] = 60.0f;
}

/// テキスト描画
void TextRenderer::Draw(FontType type,
	const wchar_t* text,
	const Vector2& position,
	const Vector4& color, 
	float rotation, 
	const Vector2& origin,
	const Vector2& scale)
{
	// 文字数取得
	float length = static_cast<float>(wcslen(text));

	TextDrawData drawData;
	drawData.type = type;
	drawData.text = text;
	drawData.position = position;
	drawData.color = color;
	drawData.rotation = rotation;
	drawData.origin.x = origin.x * length * _fontScale[static_cast<int>(type)];
	drawData.origin.y = origin.x * length;
	drawData.scale = scale;
	// 描画するテキストデータを保持
	_textDrawDatas.push_back(drawData);
}

/// テキスト描画
void TextRenderer::Draw(FontType type,
	const char* text, 
	const Vector2& position,
	const Vector4& color,
	float rotation,
	const Vector2& origin,
	const Vector2& scale)
{
	TextRenderer::Draw(type,
		ToWString(text).c_str(),
		position,
		color,
		rotation,
		origin,
		scale);
}

/// テキスト描画
void TextRenderer::Draw(TextDrawData data)
{
	// 文字数取得
	float length = static_cast<float>(wcslen(data.text.c_str()));
	data.origin.x = data.origin.x * length * _fontScale[static_cast<int>(data.type)];
	data.origin.y = data.origin.x * length;
	_textDrawDatas.push_back(data);
}

/// 3Dテキスト描画
void TextRenderer::Draw3D(FontType type, 
	const wchar_t* text,
	const Vector3& position,
	const Vector4& color,
	float rotation, 
	const Vector2& origin,
	const Vector2& scale)
{
	Text3DDrawData drawData;
	drawData.type = type;
	drawData.text = ToString(text);
	drawData.position = position;
	drawData.color = color;
	drawData.rotation = rotation;
	drawData.origin = origin;
	drawData.scale = scale;
	// 描画するテキストデータを保持
	_text3DDrawDatas.push_back(drawData);
}

/// 3Dテキスト描画
void TextRenderer::Draw3D(FontType type,
	const char* text,
	const Vector3& position,
	const Vector4& color,
	float rotation, 
	const Vector2& origin,
	const Vector2& scale)
{
	TextRenderer::Draw3D(type,
		ToWString(text).c_str(),
		position,
		color,
		rotation,
		origin,
		scale);
}

/// 3Dテキスト描画
void TextRenderer::Draw3D(Text3DDrawData data)
{
	_text3DDrawDatas.push_back(data);
}

/// 描画実行
void TextRenderer::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection,
	float viewportWidth, float viewportHeight)
{
	_spriteBatch->Begin();
	for (const auto& drawData : _textDrawDatas)
	{
		auto& spriteFont = _spriteFonts[static_cast<int>(drawData.type)];
		spriteFont->DrawString(
			_spriteBatch.get(), 
			drawData.text.c_str(),
			drawData.position,
			DirectX::XMLoadFloat4(&drawData.color),
			drawData.rotation,
			drawData.origin,
			drawData.scale);
	}
	// 描画後にクリア
	_textDrawDatas.clear();
	// 3Dテキスト描画
	for (const auto& drawData : _text3DDrawDatas)
	{
		auto& spriteFont = _spriteFonts[static_cast<int>(drawData.type)];

		// 3Dテキストは位置をスクリーン座標に変換
		Vector3 screenPosition{};
		// スクリーン座標に変換
		DirectX::XMStoreFloat3(&screenPosition,
			DirectX::XMVector3Project(
				DirectX::XMLoadFloat3(&drawData.position),
				0.0f, 0.0f,
				viewportWidth, viewportHeight,
				0.0f, 1.0f,
				DirectX::XMLoadFloat4x4(&projection),
				DirectX::XMLoadFloat4x4(&view),
				DirectX::XMMatrixIdentity()));
		// z値確認
		if (screenPosition.z < 0.0f || screenPosition.z > 1.0f)
			continue; // スクリーン外なら描画しない

		spriteFont->DrawString(
			_spriteBatch.get(),
			drawData.text.c_str(),
			Vector2(screenPosition.x, screenPosition.y),
			DirectX::XMLoadFloat4(&drawData.color),
			drawData.rotation,
			drawData.origin,
			drawData.scale);
	}
	// 描画後にクリア
	_text3DDrawDatas.clear();
	_spriteBatch->End();
}

void TextRenderer::TextDrawData::DrawGui(const char* label)
{
	if (ImGui::TreeNode(label))
	{
		std::string textUtf8 = ToString(text);
		ImGui::InputText(u8"テキスト", &textUtf8);
		text = ToUtf16(textUtf8);
		ImGui::DragFloat2(u8"位置",		&position.x, 1.0f);
		ImGui::ColorEdit4(u8"色",		&color.x);
		ImGui::DragFloat(u8"回転",		&rotation, 0.1f);
		ImGui::DragFloat2(u8"原点(0~1)",	&origin.x, 0.1f);
		ImGui::DragFloat2(u8"スケール",	&scale.x, 0.1f);
		ImGui::TreePop();
	}
}

void TextRenderer::Text3DDrawData::DrawGui(const char* label)
{
	if (ImGui::TreeNode(label))
	{
		ImGui::InputText(u8"テキスト", &text);
		ImGui::DragFloat3(u8"位置",		&position.x, 1.0f);
		ImGui::ColorEdit4(u8"色",		&color.x);
		ImGui::DragFloat(u8"回転",		&rotation, 0.1f);
		ImGui::DragFloat2(u8"原点(0~1)",	&origin.x, 0.1f);
		ImGui::DragFloat2(u8"スケール",	&scale.x, 0.1f);
		ImGui::TreePop();
	}
}
