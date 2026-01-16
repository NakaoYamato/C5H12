#include "BoxCollider.h"

#include <imgui.h>

#include "../../Scene/Scene.h"

// 開始処理
void BoxCollider::Start()
{
	// コライダーの登録
	GetActor()->GetScene()->GetCollisionManager().RegisterBoxCollider(this);
}

// デバッグ描画処理
void BoxCollider::DebugRender(const RenderContext& rc)
{
	// 有効でなければ描画しない
	if (!IsActive())
		return;

	Debug::Renderer::DrawBox(
		GetPosition().TransformCoord(GetActor()->GetTransform().GetMatrix()),
		GetActor()->GetTransform().GetWorldAngle(),
		GetHalfSize(),
		IsTrigger() ? Vector4::Blue : Vector4::Green);
}

// 削除時処理
void BoxCollider::OnDelete()
{
	// コライダーの削除
	GetActor()->GetScene()->GetCollisionManager().UnregisterBoxCollider(this);
}

// 削除時処理
void BoxCollider::DrawGui()
{
	ColliderBase::DrawGui();
	ImGui::DragFloat3(u8"中心座標", &_center.x, 0.01f);
	ImGui::DragFloat3(u8"半辺長", &_halfSize.x, 0.01f);
}

#pragma region 入出力
// ファイル読み込み
bool BoxCollider::LoadFromFile()
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
	_halfSize = jsonData.value("halfSize", Vector3(1.0f, 1.0f, 1.0f));
	return true;
}

// ファイル保存
bool BoxCollider::SaveToFile()
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
	jsonData["halfSize"]	= _halfSize;

	return Exporter::SaveJsonFile(filePath.c_str(), jsonData);
}
#pragma endregion
