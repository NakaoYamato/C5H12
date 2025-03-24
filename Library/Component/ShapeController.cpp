#include "ShapeController.h"

#include <imgui.h>

void ShapeController::DrawGui()
{
	static const char* shapTypeName[] =
	{
		u8"Box",
		u8"Sphere",
		u8"Capsule",
	};
	int type = static_cast<int>(_type);
	if (ImGui::Combo(u8"使用するシェイプタイプ", &type, shapTypeName, _countof(shapTypeName)))
	{
		_type = static_cast<ShapeType>(type);
		// 変更されていたらモデル変更
		LoadModel(GetShapeModelFilename(_type));
	}
	ImGui::DragFloat("radius", &_radius, 0.01f);
	ImGui::DragFloat("height", &_height, 0.01f);
	ModelRenderer::DrawGui();
}

// タイプに合わせたモデルのファイルパス取得
const char* ShapeController::GetShapeModelFilename(ShapeType type) const
{
	static const char* filenames[] =
	{
		"./Data/Model/Shape/Box.fbx",
		"./Data/Model/Shape/Sphere.fbx",
		"./Data/Model/Shape/Capsule.fbx",
	};
	return filenames[static_cast<int>(type)];
}
