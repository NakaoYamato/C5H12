#include "PlayerArmorController.h"

#include "../../Library/Exporter/Exporter.h"
#include "../../Library/Algorithm/Converter.h"

#include <Mygui.h>

// 生成時処理
void PlayerArmorController::OnCreate()
{
	_userDataManager = ResourceManager::Instance().GetResourceAs<UserDataManager>("UserDataManager");
}

// 更新処理
void PlayerArmorController::Update(float elapsedTime)
{
}

// GUI描画
void PlayerArmorController::DrawGui()
{
	if (ImGui::Button(u8"保存"))
	{
		SaveArmorData();
	}
	//if (ImGui::InputInt(u8"頭防具インデックス", &_armorIndices[static_cast<int>(ArmorType::Head)]))
	//{
	//	SetArmorIndex(ArmorType::Head, _armorIndices[static_cast<int>(ArmorType::Head)]);
	//}
	//if (ImGui::InputInt(u8"胴防具インデックス", &_armorIndices[static_cast<int>(ArmorType::Chest)]))
	//{
	//	SetArmorIndex(ArmorType::Chest, _armorIndices[static_cast<int>(ArmorType::Chest)]);
	//}
	//if (ImGui::InputInt(u8"腕防具インデックス", &_armorIndices[static_cast<int>(ArmorType::Arm)]))
	//{
	//	SetArmorIndex(ArmorType::Arm, _armorIndices[static_cast<int>(ArmorType::Arm)]);
	//}
	//if (ImGui::InputInt(u8"腰防具インデックス", &_armorIndices[static_cast<int>(ArmorType::Waist)]))
	//{
	//	SetArmorIndex(ArmorType::Waist, _armorIndices[static_cast<int>(ArmorType::Waist)]);
	//}
	//if (ImGui::InputInt(u8"脚防具インデックス", &_armorIndices[static_cast<int>(ArmorType::Leg)]))
	//{
	//	SetArmorIndex(ArmorType::Leg, _armorIndices[static_cast<int>(ArmorType::Leg)]);
	//}
}

// 防具インデックス取得
int PlayerArmorController::GetArmorIndex(ArmorType type) const
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return -1;

	return userDataManager->GetEquippedArmorIndex(type);
}

// 防具インデックス設定
void PlayerArmorController::SetArmorIndex(ArmorType type, int index)
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;

	// データがあるか確認
	UserDataManager::ArmorUserData* userData = userDataManager->GetAcquiredArmorData(type, index);

	// アクターが存在する場合は更新
	auto armorActor = GetArmorActor(type);
	if (armorActor)
	{
		if (userData)
			armorActor->BuildData(userData->GetBaseData(), index);
		else
			armorActor->BuildData(nullptr, -1);
	}
}

void PlayerArmorController::SaveArmorData()
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;

	userDataManager->SaveToFile();
}
