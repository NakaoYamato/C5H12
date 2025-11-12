#pragma once

#include <string>

class ResourceBase
{
public:
	ResourceBase() {}
	virtual ~ResourceBase() {}

	// 名前取得
	virtual std::string GetName() const = 0;
	// ファイルパス取得
	virtual std::string GetFilePath() const = 0;

	// 初期化処理
	virtual bool Initialize() { return true; }

	// ファイル読み込み
	virtual bool LoadFromFile() = 0;
	// ファイル保存
	virtual bool SaveToFile() = 0;

	// Gui描画
	virtual void DrawGui() {}

};