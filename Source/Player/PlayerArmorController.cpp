#include "PlayerArmorController.h"

#include "../../Library/Exporter/Exporter.h"
#include "../../Library/Algorithm/Converter.h"

#include <Mygui.h>

// 生成時処理
void PlayerArmorController::OnCreate()
{
	_damageable = GetActor()->GetComponent<Damageable>();
	_userDataManager = ResourceManager::Instance().GetResourceAs<UserDataManager>("UserDataManager");
}

// 更新処理
void PlayerArmorController::Update(float elapsedTime)
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;

	// 防御力を設定
	float totalDefense = 0.0f;
	for (size_t i = 0; i <= static_cast<size_t>(ArmorType::Leg); ++i)
	{
		UserDataManager::ArmorUserData* userData = userDataManager->GetEquippedArmorData(static_cast<ArmorType>(i));
		if (userData)
		{
			totalDefense += userData->GetBaseData()->defense;
		}
	}
	if (auto damageable = _damageable.lock())
	{
		damageable->SetDefense(totalDefense);
	}
}

// GUI描画
void PlayerArmorController::DrawGui()
{
	if (ImGui::Button(u8"保存"))
	{
		SaveArmorData();
	}
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
