#include "EntryZone.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/AI/MetaAI.h"

#include <imgui.h>

// 開始処理
void EntryZone::Start()
{
	// メタAIを取得
	auto metaAIActor = GetActor()->GetScene()->GetActorManager().FindByName("MetaAI", ActorTag::System);
	if (metaAIActor)
	{
		// メタAIに登録
		auto metaAI = metaAIActor->GetComponent<MetaAI>();
		if (metaAI)
		{
			metaAI->RegisterEntryZone(shared_from_this());
		}
	}
	// データ読み込み
	LoadFromFile();
}

// GUI描画
void EntryZone::DrawGui()
{
	ImGui::Combo(u8"陣営", reinterpret_cast<int*>(&_faction), "Player\0Enemy\0Neutral\0\0");
	ImGui::DragFloat3(u8"中心位置", &_center.x, 0.1f, -1000.0f, 1000.0f);
	Vector3 deg = Vector3::ToDegrees(_angle);
	ImGui::DragFloat3(u8"向き", &deg.x, 1.0f, 0.0f, 360.0f);
	_angle = Vector3::ToRadians(deg);
	ImGui::DragFloat(u8"半径", &_radius, 0.1f, 0.0f, 1000.0f);
	ImGui::Checkbox(u8"デバッグレンダリング", &_debugRender);
}

// デバッグ表示
void EntryZone::DebugRender(const RenderContext& rc)
{
	if (!_debugRender)
		return;
	Vector4 color = Vector4::White;
	switch (_faction)
	{
	case Targetable::Faction::Player:
		color = Vector4::Blue;
		break;
	case Targetable::Faction::Enemy:
		color = Vector4::Red;
		break;
	case Targetable::Faction::Neutral:
		color = Vector4::Green;
		break;
	default:
		color = Vector4::White;
		break;
	}
	Debug::Renderer::DrawSphere(
		GetActor()->GetTransform().GetWorldPosition() + _center,
		_radius,
		color);
}

#pragma region 入出力
// ファイル読み込み
bool EntryZone::LoadFromFile()
{
	// 現在のシーン名、アクター名からファイルパスを生成
	std::string filePath = this->GetDirectory();
	filePath += this->GetName();
	filePath += ".json";

	nlohmann::json jsonData;
	if (!Exporter::LoadJsonFile(filePath.c_str(), &jsonData))
		return false;
	// 各データ読み込み
	_faction = jsonData.value("faction", Targetable::Faction::Neutral);
	_center = jsonData.value("center", Vector3{});
	_angle = jsonData.value("angle", Vector3{});
	_radius = jsonData.value("radius", 5.0f);

	return true;
}

// ファイル保存
bool EntryZone::SaveToFile()
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
	jsonData["faction"] = _faction;
	jsonData["center"] = _center;
	jsonData["angle"] = _angle;
	jsonData["radius"] = _radius;

	return Exporter::SaveJsonFile(filePath.c_str(), jsonData);
}
#pragma endregion
