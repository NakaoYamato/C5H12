#include "SphereCollider.h"

#include <imgui.h>

#include "../../Scene/Scene.h"

void SphereCollider::Start()
{
	// コライダーの登録
	GetActor()->GetScene()->GetCollisionManager().RegisterSphereCollider(this);
}

// デバッグ描画処理
void SphereCollider::DebugRender(const RenderContext& rc)
{
	// 有効でなければ描画しない
	if (!IsActive())
		return;

	Debug::Renderer::DrawSphere(
		GetPosition().TransformCoord(GetActor()->GetTransform().GetMatrix()),
		GetRadius(),
		IsTrigger() ? Vector4::Blue : Vector4::Green);
}

// 削除時処理
void SphereCollider::OnDelete()
{
	// コライダーの削除
	GetActor()->GetScene()->GetCollisionManager().UnregisterSphereCollider(this);
}

// GUI描画
void SphereCollider::DrawGui()
{
	ColliderBase::DrawGui();
	ImGui::DragFloat3(u8"中心座標", &_center.x, 0.01f);
	ImGui::DragFloat(u8"半径", &_radius, 0.01f, 0.0f, 100.0f);
}

#pragma region 入出力
// ファイル読み込み
bool SphereCollider::LoadFromFile()
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
	_center = jsonData.value("center", Vector3(0.0f, 0.0f, 0.0f));
	_radius = jsonData.value("radius", 1.0f);
	return true;
}

// ファイル保存
bool SphereCollider::SaveToFile()
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
	jsonData["center"]		= _center;
	jsonData["radius"]		= _radius;

	return Exporter::SaveJsonFile(filePath.c_str(), jsonData);
}
#pragma endregion
