#pragma once

#include "../../Library/Actor/Actor.h"
#include "../../Library/Actor/ActorFactory.h"

class StageManager : public Actor
{
public:
	// 配置情報
	struct EnvironmentLayout
	{
		std::weak_ptr<Actor> actor;
		std::string actorType = "";
		std::string parent = "";
		Vector3 position = Vector3::Zero;
		Vector3 angle = Vector3::Zero;
		Vector3 scale = Vector3::One;
	};

public:
	StageManager() = default;
	~StageManager() override = default;
	// 生成時処理
	void OnCreate() override;
	// 開始処理
	void OnStart() override;
	// 更新処理
	void OnUpdate(float elapsedTime) override;
	// GUI描画
	void OnDrawGui() override;

#pragma region 入出力
	// ファイルパス取得
	std::string GetFilePath() override;

	// ファイル読み込み
	bool LoadFromFile() override;
	// ファイル保存
	bool SaveToFile() override;
#pragma endregion

private:
	// 環境アクター生成
	std::shared_ptr<Actor> CreateEnvironmentActor(
		std::string actorType,
		std::string parent,
		const Vector3& position,
		const Vector3& angle,
		const Vector3& scale);

	// 編集用アクター生成
    void CreateEditingEnvironmentActors();

	// 編集用GUI描画
	void DrawEditingGui();

	// 編集中の環境アクターGUI表示
	void DrawEditingEnvironmentActorGui(int index);

private:
	// ActorFactoryへの参照
	std::weak_ptr<ActorFactory> _actorFactory;

	// 生成する環境アクターのインデックス
	int _creationEnvironmentIndex = 0;
#pragma region 編集
	// 編集中かどうか
	bool _isEditing = false;

	// 選択している環境アクターのインデックス
	int _selectedEnvironmentIndex = -1;
	// レイの長さ
	float _rayLength = 1000.0f;
	// 交差点のワールド座標
	Vector3 _intersectionWorldPoint = Vector3::Zero;
	// 生成する環境アクターの回転値
	Vector3 _creationAngle = Vector3::Zero;
	// 生成する環境アクターのサイズ
	Vector3 _creationScale = Vector3::One;
	// 交差したアクターの参照
	Actor* _intersectedActor = nullptr;

	// 編集で表示する用の環境アクターのリスト
	std::vector<std::weak_ptr<Actor>> _editingEnvironmentActors;
	// 生成した環境アクター配置情報
	std::vector<EnvironmentLayout> _createdEnvironmentLayouts;

	// ダブルクリック判定
	bool _isDoubleClick = false;
	// ダブルクリックの閾値
	float _doubleClickThreshold = 0.3f;
	// 左クリック時間
	float _leftClickTime = 0.0f;

	// 最後に生成したアクター番号
	int _lastCreatedActorIndex = -1;
#pragma endregion
};
