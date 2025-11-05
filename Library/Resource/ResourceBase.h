#pragma once

#include <string>

class ResourceBase
{
public:
	ResourceBase() {}
	virtual ~ResourceBase() {}

	// 名前取得
	virtual const std::string& GetName() const = 0;
	// ファイルパス取得
	virtual const std::string& GetFilePath() const = 0;

	// ファイル読み込み
	virtual bool LoadFromFile() = 0;
	// ファイル保存
	virtual bool SaveToFile() = 0;

	// Gui描画
	virtual void DrawGui() {}

};