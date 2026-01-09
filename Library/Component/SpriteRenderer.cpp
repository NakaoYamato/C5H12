#include "SpriteRenderer.h"

#include "../../Library/Algorithm/Converter.h"
#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Collision/CollisionMath.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/Exporter/Exporter.h"

#include <fstream>
#include <Mygui.h>

// 生成時処理
void SpriteRenderer::OnCreate()
{
	// 親がUIActorであればRectTransformを取得
	UIActor* uiActor = dynamic_cast<UIActor*>(GetActor().get());
	if (uiActor)
	{
		_myRectTransform = &uiActor->GetRectTransform();
	}

	// データ読み込み
	_isLoaded = LoadFromFile();
}

// 更新処理
void SpriteRenderer::Update(float elapsedTime)
{
	for (auto& [name, spriteData] : _sprites)
	{
		if (!spriteData._parentName.empty())
		{
			// 親のRectTransformを取得
			if (_sprites.find(spriteData._parentName) != _sprites.end())
			{
				auto& parentRectTransform = _sprites.at(spriteData._parentName).GetRectTransform();
				spriteData.UpdateTransform(&parentRectTransform);
			}
		}
		else
			spriteData.UpdateTransform(_myRectTransform);
	}
}

// 3D描画後の描画処理
void SpriteRenderer::DelayedRender(const RenderContext& rc)
{
	// ステンシルをクリア
	rc.deviceContext->ClearDepthStencilView(rc.depthStencilView, D3D11_CLEAR_STENCIL, 1.0f, 0);

	for (const auto& spriteName : _spriteDrawOrder)
	{
		if (_sprites.find(spriteName) == _sprites.end())
            continue;
        auto& spriteData = _sprites.at(spriteName);
		if (!spriteData._isActive)
			continue;

		SpriteRender(spriteName, rc);
    }
}
// GUI描画
void SpriteRenderer::DrawGui()
{
	static std::string newSpriteName = "NewSprite";
	ImGui::InputText(u8"スプライト名", &newSpriteName);
	if (ImGui::Button(u8"追加"))
	{
		std::string resultPath = "";
		{
			// ダイアログを開く
			std::string filepath;
			std::string currentDirectory;
			Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, ImGui::TextureFilter);
			// ファイルを選択したら
			if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
			{
				// 相対パス取得
				try
				{
					// 相対パス取得
					std::filesystem::path path = std::filesystem::relative(filepath, currentDirectory);
					resultPath = path.u8string();
				}
				catch (...)
				{
					// 相対パス取得に失敗した場合は絶対パスを使用
					resultPath = filepath;
				}
			}
		}

		LoadTexture(newSpriteName, ToWString(resultPath).c_str(), Sprite::CenterCenter);
	}
	ImGui::Separator();

	for (auto& [name, spriteData] : _sprites)
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::Text(u8"親:");
			ImGui::SameLine();
			ImGui::InputText("##parentName", &spriteData._parentName);

			ImGui::Separator();
			spriteData.DrawGui();
			spriteData._textData.DrawGui(u8"テキスト");
			ImGui::Separator();
			if (ImGui::Button(u8"削除"))
			{
				_sprites.erase(name);
				// 描画順リストからも削除
				_spriteDrawOrder.erase(std::remove(_spriteDrawOrder.begin(), _spriteDrawOrder.end(), name), _spriteDrawOrder.end());
				ImGui::TreePop();
				break;
			}
			ImGui::TreePop();
		}
	}
	ImGui::DragFloat(u8"全体透明度", &_overallAlpha, 0.01f, 0.0f, 1.0f);
	ImGui::Separator();

	if (ImGui::TreeNode(u8"描画順"))
	{
		for (int n = 0; n < _spriteDrawOrder.size(); n++)
		{
			ImGui::PushID(n);

			ImGui::Selectable(_spriteDrawOrder[n].c_str(), false);

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				// ペイロードとして「現在のインデックス(int)」を渡す
				ImGui::SetDragDropPayload("DND_SPRITE_ORDER", &n, sizeof(int));

				// ドラッグ中にマウスカーソルの横に表示されるプレビュー
				ImGui::Text("Move %s", _spriteDrawOrder[n].c_str());

				ImGui::EndDragDropSource();
			}

			// ドロップ処理
			if (ImGui::BeginDragDropTarget())
			{
				// ペイロードを受け取る
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_SPRITE_ORDER"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int source_n = *(const int*)payload->Data;
					int target_n = n;

					// 自分自身へのドロップでなければ入れ替え処理を行う
					if (source_n != target_n)
					{
						// 移動させる要素をコピー
						std::string temp = _spriteDrawOrder[source_n];

						// 元の位置から削除
						_spriteDrawOrder.erase(_spriteDrawOrder.begin() + source_n);

						_spriteDrawOrder.insert(_spriteDrawOrder.begin() + target_n, temp);
					}
				}
				ImGui::EndDragDropTarget();
			}

			// 右クリックされたとき
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Delete"))
				{
					// 描画順リストから削除
					_spriteDrawOrder.erase(_spriteDrawOrder.begin() + n);

					ImGui::EndPopup();
					ImGui::PopID();
					break;
				}
				ImGui::EndPopup();
			}

			ImGui::PopID();
		}

		ImGui::TreePop();
	}
	ImGui::Separator();
}
// 画像読み込み
void SpriteRenderer::LoadTexture(const std::string& spriteName,
	const wchar_t* filename,
	Sprite::CenterAlignment alignment)
{
	_sprites[spriteName].LoadTexture(filename, alignment);
    _spriteDrawOrder.push_back(spriteName);
}
// 画像との当たり判定
bool SpriteRenderer::IsHit(const std::string& name, const Vector2& pos) const
{
    if (_sprites.find(name) != _sprites.end())
    {
        const auto& spriteData = _sprites.at(name);
		return spriteData.IsHit(pos);
    }
	return false;
}
// ファイル読み込み
bool SpriteRenderer::LoadFromFile()
{
	// 現在のシーン名、アクター名からファイルパスを生成
	std::string filePath = this->GetDirectory();
	filePath += this->GetName();
	filePath += ".json";

	nlohmann::json jsonData;
	if (!Exporter::LoadJsonFile(filePath.c_str(), &jsonData))
		return false;

	// 各スプライトデータ読みこみ
	size_t spriteSize = jsonData["spriteSize"].get<size_t>();
	for (size_t index = 0; index < spriteSize; ++index)
	{
		std::string label = "sprite_" + std::to_string(index);
		auto& sub = jsonData[label];

		std::string name = sub.value("name", "");
		_sprites[name]._parentName = sub.value("parentName", "");

		// テクスチャデータ
		std::string textureFilePath = sub.value("textureFilePath", "");
		_sprites[name].LoadTexture(ToWString(textureFilePath).c_str(),
			static_cast<Sprite::CenterAlignment>(sub.value("centerAlignment", Sprite::CenterAlignment::CenterCenter)));

		// トランスフォームデータ
		_sprites[name].GetRectTransform() = sub.value("RectTransform", RectTransform());

		// マテリアルデータ
		_sprites[name].GetMaterial().LoadFromFile(sub);

		_sprites[name].SetCenterAlignment(	sub.value("centerAlignment",	Sprite::CenterAlignment::CenterCenter));
		_sprites[name].SetTexPos(			sub.value("texPos",				_sprites[name].GetTexPos()));
		_sprites[name].SetTexSize(			sub.value("texSize",			_sprites[name].GetTexSize()));
		_sprites[name].SetCenter(			sub.value("center",				_sprites[name].GetCenter()));
		_sprites[name].SetColor(			sub.value("color",				_sprites[name].GetColor()));
		_sprites[name].SetDepthState(		sub.value("depthState",			DepthState::TestAndWrite));
		_sprites[name].SetStencil(			sub.value("stencil",			0));

		// 文字データ
		std::string text = sub.value("text", "");
		_sprites[name]._textData.type		= sub.value("textType",		TextRenderer::TextDrawData().type);
		_sprites[name]._textData.text		= ToUtf16(text);
		_sprites[name]._textData.position	= sub.value("textPosition", TextRenderer::TextDrawData().position);
		_sprites[name]._textData.color		= sub.value("textColor",	TextRenderer::TextDrawData().color);
		_sprites[name]._textData.rotation	= sub.value("textRotation", TextRenderer::TextDrawData().rotation);
		_sprites[name]._textData.origin		= sub.value("textOrigin",	TextRenderer::TextDrawData().origin);
		_sprites[name]._textData.scale		= sub.value("textScale",	TextRenderer::TextDrawData().scale);
	}

	// 描画順読みこみ
	size_t drawOrderSize = jsonData["drawOrderSize"].get<size_t>();
	for (size_t index = 0; index < drawOrderSize; ++index)
	{
		std::string label = "drawOrder_" + std::to_string(index);
		std::string spriteName = jsonData[label].get<std::string>();
		_spriteDrawOrder.push_back(spriteName);
	}

	return true;
}
// ファイル保存
bool SpriteRenderer::SaveToFile()
{
	// 現在のシーン名、アクター名からファイルパスを生成
	std::string filePath = this->GetDirectory();

	// ディレクトリ確保
	std::filesystem::path outputDirPath(filePath);
	if (!std::filesystem::exists(outputDirPath))
	{
		// なかったらディレクトリ作成
		std::filesystem::create_directories(outputDirPath);
	}

	filePath += this->GetName();
	filePath += ".json";

	nlohmann::json jsonData;
	jsonData["spriteSize"] = _sprites.size();

	// 各スプライトデータ保存
	size_t index = 0;
	for (auto& [name, spriteData] : _sprites)
	{
		std::string label = "sprite_" + std::to_string(index);
		auto& sub = jsonData[label];

		sub["name"] = name;
		sub["parentName"] = spriteData._parentName;

		// テクスチャデータ
		sub["textureFilePath"] = ToString(spriteData.GetTexture().GetFilepath());

		// トランスフォームデータ
		sub["RectTransform"] = spriteData.GetRectTransform();

		// マテリアルデータ
		spriteData.GetMaterial().SaveToFile(sub);

		sub["centerAlignment"]	= spriteData.GetCenterAlignment();
		sub["texPos"]			= spriteData.GetTexPos();
		sub["texSize"]			= spriteData.GetTexSize();
		sub["center"]			= spriteData.GetCenter();
		sub["color"]			= spriteData.GetColor();
		sub["depthState"]		= spriteData.GetDepthState();
		sub["stencil"]			= spriteData.GetStencil();

		// 文字データ
		sub["textType"]			= spriteData._textData.type;
		sub["text"]				= ToString(spriteData._textData.text);
		sub["textPosition"]		= spriteData._textData.position;
		sub["textColor"]		= spriteData._textData.color;
		sub["textRotation"]		= spriteData._textData.rotation;
		sub["textOrigin"]		= spriteData._textData.origin;
		sub["textScale"]		= spriteData._textData.scale;

		index++;
	}

	// 描画順保存
	index = 0;
	jsonData["drawOrderSize"] = _spriteDrawOrder.size();
	for (const auto& spriteName : _spriteDrawOrder)
	{
		std::string label = "drawOrder_" + std::to_string(index);
		jsonData[label] = spriteName;
		index++;
	}

	return Exporter::SaveJsonFile(filePath.c_str(), jsonData);
}
// スプライト描画
void SpriteRenderer::SpriteRender(const std::string& spriteName, 
	const RenderContext& rc)
{
	auto& textureRenderer = GetActor()->GetScene()->GetTextureRenderer();
	_sprites[spriteName].Render(rc, textureRenderer, _overallAlpha);
	// 文字描画
	if (!_sprites[spriteName]._textData.text.empty())
	{
		auto& textRenderer = GetActor()->GetScene()->GetTextRenderer();
		textRenderer.Draw(_sprites[spriteName]._textData);
	}
	if (GetActor()->IsDrawingDebug() && !Debug::Input::IsActive(DebugInput::BTN_F7))
	{
		// 中心をデバッグ描画
		GetActor()->GetScene()->GetPrimitive()->Circle(rc.deviceContext,
			_sprites[spriteName].GetRectTransform().GetWorldPosition(),
			5.0f);
	}
}
