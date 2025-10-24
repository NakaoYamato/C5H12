#include "RoarController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Camera/Camera.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/PostProcess/PostProcessManager.h"

#include <imgui.h>

// 開始処理
void RoarController::Start()
{
}

// 更新処理
void RoarController::Update(float elapsedTime)
{
	// 咆哮のブラー更新
	if (_rate)
	{
		auto data = PostProcessManager::Instance().GetPostProcess(PostProcessType::RadialBlurPP)->GetCurrentData();

		data["blurCenter.x"] = (_screenPosition.x / Graphics::Instance().GetScreenWidth());
		data["blurCenter.y"] = (_screenPosition.y / Graphics::Instance().GetScreenHeight());
		data["blurRadius"] = _radius * _rate;

		PostProcessManager::Instance().GetPostProcess(PostProcessType::RadialBlurPP)->SetData(data);
	}
}

// GUI描画
void RoarController::DrawGui()
{
	ImGui::DragFloat3(u8"ワールド座標", &_worldPosition.x);
	ImGui::DragFloat2(u8"スクリーン座標", &_screenPosition.x);
	ImGui::DragFloat(u8"半径", &_radius);
	ImGui::DragFloat(u8"咆哮の強さ", &_rate);
}

void RoarController::SetWorldPosition(const Vector3& position)
{
	_worldPosition = position;
	// ワールド座標をスクリーン座標に変換
	auto camera = GetActor()->GetScene()->GetMainCamera();
	auto vec = _worldPosition.Project(
		Graphics::Instance().GetScreenWidth(),
		Graphics::Instance().GetScreenHeight(),
		camera->GetView(),
		camera->GetProjection());
	_screenPosition = Vector2(vec.x, vec.y);
}

void RoarController::Reset()
{
	auto data = PostProcessManager::Instance().GetPostProcess(PostProcessType::RadialBlurPP)->GetCurrentData();

	_rate = 0.0f;
	data["blurCenter.x"] = (_screenPosition.x / Graphics::Instance().GetScreenWidth());
	data["blurCenter.y"] = (_screenPosition.y / Graphics::Instance().GetScreenHeight());
	data["blurRadius"] = _radius * _rate;

	PostProcessManager::Instance().GetPostProcess(PostProcessType::RadialBlurPP)->SetData(data);
}
