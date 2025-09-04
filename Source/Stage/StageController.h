#pragma once

#include "../../Library/Component/Component.h"

class StageController : public Component
{
public:
	StageController(int num) : _areaNumber(num) {}
	~StageController()override {}
	// 名前取得
	const char* GetName() const override { return "StageController"; }
	// GUI描画
	void DrawGui()override;

#pragma region アクセサ
	int GetAreaNumber() const { return _areaNumber; }
	void SetAreaNumber(int num) { _areaNumber = num; }
#pragma endregion

private:
	// エリア番号
	int _areaNumber = -1;
};