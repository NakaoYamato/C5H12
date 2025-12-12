#include "WeaponData.h"

#include "../../Library/Algorithm/Converter.h"
#include <numeric>
#include <algorithm>
#include <Mygui.h>

// レア度から色取得
Vector4 WeaponData::GetRarityColor(int rarity)
{
	switch (rarity)
	{
	case 1:
		return Vector4::Gray;
	case 2:
		return Vector4::White;
	case 3:
		return Vector4::LightGreen;
	case 4:
		return Vector4::Green;
	case 5:
		return Vector4::Cyan;
	case 6:
		return Vector4::Blue;
	case 7:
		return Vector4::Purple;
	case 8:
		return Vector4::Orange;
	default:
		return Vector4::White;
	}
}

// 切れ味から色取得
Vector4 WeaponData::GetSharpnessColor(int sharpnessLevel)
{
	switch (sharpnessLevel)
	{
	case 1:
		return Vector4::Red;
	case 2:
		return Vector4::Orange;
	case 3:
		return Vector4::Yellow;
	case 4:
		return Vector4::Green;
	case 5:
		return Vector4::Blue;
	case 6:
		return Vector4::White;
	case 7:
		return Vector4::Purple;
	default:
		return Vector4::Red;
	}
}

// GUI描画
void WeaponData::DrawGui()
{
	ImGui::InputText(u8"名前", &name);
	ImGui::Text(u8"ファイルパス:");
	ImGui::SameLine();
	ImGui::Text(modelFilePath.c_str());
	ImGui::SameLine();
	if (ImGui::Button("..."))
	{
		// ダイアログを開く
		std::string filepath;
		std::string currentDirectory;
		Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, ImGui::ModelFilter);
		// ファイルを選択したら
		if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
		{
			try
			{
				// 相対パス取得
				std::filesystem::path path =
					std::filesystem::relative(filepath, currentDirectory);
				modelFilePath = path.u8string();
			}
			catch (...)
			{
				modelFilePath = filepath;
			}
		}
	}
    ImGui::Separator();

	ImGui::InputFloat(u8"攻撃力", &attack);
	ImGui::InputInt(u8"レア度", &rarity);
    ImGui::Separator();

    // 切れ味ゲージGUI描画
    ImGui::Text(u8"切れ味ゲージ設定");
    DrawSharpnessGaugeGui(sharpnessGauge, true);
}

// 切れ味ゲージGUI描画
void WeaponData::DrawSharpnessGaugeGui(std::vector<float>& sharpnessGauge, bool canEdit)
{
    ImGui::Spacing();
    // 要素数の変更
    int currentLevel = static_cast<int>(sharpnessGauge.size());
    if (canEdit)
    {
        if (ImGui::Combo(u8"レベル数", &currentLevel, u8"0\0 1\0 2\0 3\0 4\0 5\0 6\0 7\0\0"))
        {
            // 最大値制限
            if (currentLevel > WeaponSharpnessLevelMax) currentLevel = WeaponSharpnessLevelMax;
            if (currentLevel < 0) currentLevel = 0;

            // サイズ変更
            // 新規追加分は適当な値(10.0f)で初期化
            sharpnessGauge.resize(currentLevel, 10.0f);

            // 合計がMaxを超えていたら末尾から削る、あるいは全体をスケールダウンする等の補正
            float total = 0.0f;
            for (float v : sharpnessGauge) total += v;
            if (total > WeaponSharpnessGaugeMax)
            {
                // 単純にスケールダウンして収める
                float scale = WeaponSharpnessGaugeMax / total;
                for (float& v : sharpnessGauge) v *= scale;
            }
        }
    }
    else
    {
		ImGui::Text(u8"レベル数: %d", currentLevel);
    }

    // ゲージの描画と編集
    // 描画領域の確保
    ImVec2 contentRegion = ImGui::GetContentRegionAvail();
    float barWidth = contentRegion.x;
    float barHeight = 20.0f;
    ImVec2 startPos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // 背景（最大値の枠）を描画 (濃いグレー)
    drawList->AddRectFilled(
        startPos,
        ImVec2(startPos.x + barWidth, startPos.y + barHeight),
        IM_COL32(50, 50, 50, 255)
    );

    float currentPixelX = 0.0f;

    ImGui::PushID("SharpnessGaugeEditor");

    for (int i = 0; i < sharpnessGauge.size(); ++i)
    {
        // 値をピクセル幅に変換
        float widthPixel = (sharpnessGauge[i] / WeaponSharpnessGaugeMax) * barWidth;

        // 色の取得と変換 (GetSharpnessColorは1始まりのindexを想定しているため i+1)
        Vector4 vecColor = GetSharpnessColor(i + 1);
        ImU32 rectColor = IM_COL32(
            static_cast<int>(vecColor.x * 255),
            static_cast<int>(vecColor.y * 255),
            static_cast<int>(vecColor.z * 255),
            255
        );

        // 各色の矩形を描画
        ImVec2 pMin = ImVec2(startPos.x + currentPixelX, startPos.y);
        ImVec2 pMax = ImVec2(pMin.x + widthPixel, pMin.y + barHeight);
        drawList->AddRectFilled(pMin, pMax, rectColor);

        ImGui::PushID(i);
        ImGui::SetCursorScreenPos(ImVec2(pMax.x - 3.0f, pMin.y));

        // 幅6pxの透明ボタンを配置してドラッグ判定を行う
        ImGui::InvisibleButton("##Handle", ImVec2(6.0f, barHeight));

        // ホバー時にリサイズカーソルを表示
        if (ImGui::IsItemHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            ImGui::SetTooltip("%.1f", sharpnessGauge[i]);
        }

        if (canEdit)
        {
            // ドラッグ中
            if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
            {
                float dragDeltaX = ImGui::GetIO().MouseDelta.x;

                // ピクセル移動量を数値移動量に変換
                float valueDelta = (dragDeltaX / barWidth) * WeaponSharpnessGaugeMax;

                // 自身以外の合計値を計算
                float otherSum = 0.0f;
                for (int j = 0; j < sharpnessGauge.size(); ++j)
                {
                    if (i != j) otherSum += sharpnessGauge[j];
                }

                // 上限 = 全体Max - 他の合計
                float maxAllowed = WeaponSharpnessGaugeMax - otherSum;

                // 値を更新 (0 ～ maxAllowed の範囲に収める)
                sharpnessGauge[i] += valueDelta;
                if (sharpnessGauge[i] < 0.0f) sharpnessGauge[i] = 0.0f;
                if (sharpnessGauge[i] > maxAllowed) sharpnessGauge[i] = maxAllowed;
            }
        }

        ImGui::PopID();

        // 次の描画開始位置へ
        currentPixelX += (sharpnessGauge[i] / WeaponSharpnessGaugeMax) * barWidth;
    }

    ImGui::PopID();

    // 描画後のカーソル位置調整（InvisibleButton等でカーソルが変な位置にあるため）
    ImGui::SetCursorScreenPos(ImVec2(startPos.x, startPos.y + barHeight + 5.0f));
}

// データ保存
void WeaponData::Load(nlohmann::json_abi_v3_12_0::json& json)
{
	type = static_cast<WeaponType>(json.value("type", static_cast<int>(type)));
	name = json.value("name", name);
	modelFilePath = json.value("modelFilePath", modelFilePath);
	attack = json.value("attack", attack);
	rarity = json.value("rarity", rarity);
	sharpnessGauge = json.value("sharpnessGauge", sharpnessGauge);
}

// データ出力
void WeaponData::Save(nlohmann::json_abi_v3_12_0::json& json) const
{
	json["type"] = static_cast<int>(type);
	json["name"] = name;
	json["modelFilePath"] = modelFilePath;
	json["attack"] = attack;
	json["rarity"] = rarity;
	json["sharpnessGauge"] = sharpnessGauge;
}
