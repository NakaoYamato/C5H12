#pragma once

#include "../../Library/Component/Component.h"
#include "../../Source/Common/Targetable.h"

#include <vector>
#include <memory>
#include <functional>

class MetaAI : public Component
{
public:
	MetaAI() {}
	~MetaAI() override = default;
	// 名前取得
	const char* GetName() const override { return "MetaAI"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

	/// <summary>
	/// ターゲット登録
	/// </summary>
	/// <param name="targetable"></param>
	void RegisterTargetable(std::weak_ptr<Targetable> targetable);

	/// <summary>
	/// ターゲット削除
	/// </summary>
	/// <param name="targetable"></param>
	void RemoveTargetable(std::weak_ptr<Targetable> targetable);

	/// <summary>
	/// ターゲット検索
	/// </summary>
	/// <param name="faction">検索する対象の陣営</param>
	/// <param name="searchPosition">検索する場所</param>
	/// <param name="searchRadius">検索する範囲</param>
	/// <param name="searchFaction">検索関数　引数は検索対象</param>
	/// <returns></returns>
	Targetable* SearchTarget(Targetable::Faction faction,
		const Vector3& searchPosition,
		float searchRadius,
		const std::function<bool(Targetable*)>& searchFaction = nullptr) const;

private:
	// ターゲット可能なオブジェクトのリスト
	std::vector<std::weak_ptr<Targetable>> _targetables;
};