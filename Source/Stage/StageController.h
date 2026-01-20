#pragma once

#include "../../Library/Component/Component.h"

#include "EntryZone.h"

class StageController : public Component, public std::enable_shared_from_this<StageController>
{
public:
	StageController(int num) : _areaNumber(num) {}
	~StageController()override {}
	// 名前取得
	const char* GetName() const override { return "StageController"; }
	// 開始時処理
	void Start() override;
	// GUI描画
	void DrawGui()override;

#pragma region アクセサ
	int GetAreaNumber() const { return _areaNumber; }
	void SetAreaNumber(int num) { _areaNumber = num; }

	// エントリーゾーンリスト取得
	std::vector<std::weak_ptr<EntryZone>>& GetEntryZones() { return _entryZones; }
#pragma endregion

private:
	// エリア番号
	int _areaNumber = -1;

	// エントリーゾーンリスト
	std::vector<std::weak_ptr<EntryZone>> _entryZones;
};