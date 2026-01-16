#include "CapsuleCollider.h"

#include <imgui.h>

#include "../../Scene/Scene.h"

// 開始処理
void CapsuleCollider::Start()
{
	// コライダーの登録
	GetActor()->GetScene()->GetCollisionManager().RegisterCapsuleCollider(this);
}

// デバッグ描画処理
void CapsuleCollider::DebugRender(const RenderContext& rc)
{
	// 有効でなければ描画しない
	if (!IsActive())
		return;

	Debug::Renderer::DrawCapsule(
		GetStart().TransformCoord(GetActor()->GetTransform().GetMatrix()),
		GetEnd().TransformCoord(GetActor()->GetTransform().GetMatrix()),
		GetRadius(),
		IsTrigger() ? Vector4::Blue : Vector4::Green);
}

// 削除時処理
void CapsuleCollider::OnDelete()
{
	// コライダーの削除
	GetActor()->GetScene()->GetCollisionManager().UnregisterCapsuleCollider(this);
}

// GUI描画
void CapsuleCollider::DrawGui()
{
	ColliderBase::DrawGui();
	ImGui::DragFloat3(u8"開始座標", &_start.x, 0.01f);
	ImGui::DragFloat3(u8"終了座標", &_end.x, 0.01f);
	ImGui::DragFloat(u8"半径", &_radius, 0.01f);
}

#pragma region 入出力
// ファイル読み込み
bool CapsuleCollider::LoadFromFile()
{
	// 現在のシーン名、アクター名からファイルパスを生成
	std::string filePath = this->GetDirectory();
	filePath += this->GetName();
	filePath += ".json";

	nlohmann::json jsonData;
	if (!Exporter::LoadJsonFile(filePath.c_str(), &jsonData))
		return false;
	// 各データ読み込み
	SetLayer(jsonData.value("layer", CollisionLayer::None));
	SetLayerMask(jsonData.value("layerMask", CollisionLayerMaskAll));
	SetActive(jsonData.value("isActive", true));
	SetTrigger(jsonData.value("isTrigger", false));
	_start = jsonData.value("start", Vector3(0.0f, 0.0f, 0.0f));
	_end = jsonData.value("end", Vector3(0.0f, 1.0f, 0.0f));
	_radius = jsonData.value("radius", 1.0f);
	return true;
}

// ファイル保存
bool CapsuleCollider::SaveToFile()
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
	jsonData["layer"]		= GetLayer();
	jsonData["layerMask"]	= GetLayerMask();
	jsonData["isActive"]	= IsActive();
	jsonData["isTrigger"]	= IsTrigger();
	jsonData["start"]		= _start;
	jsonData["end"]			= _end;
	jsonData["radius"]		= _radius;

	return Exporter::SaveJsonFile(filePath.c_str(), jsonData);
}
#pragma endregion
