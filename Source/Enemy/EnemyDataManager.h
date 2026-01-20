#pragma once

#include "../../Library/Resource/ResourceManager.h"
#include "../../Library/Actor/Actor.h"

#include <unordered_map>
#include <mutex>

class EnemyDataManager : public ResourceBase
{
public:
	using MakeFunc = std::function<std::shared_ptr<Actor>()>;

	// 敵データ
	struct EnemyData
	{
		// 生成関数
		MakeFunc	makeFunc;
		// モデルパス
		std::string modelFilePath;
	};

	struct EnemyModelData
	{
		std::shared_ptr<Model>	model;
		// モデル読み込み時の排他制御用ミューテックス
		std::mutex				mutex;
		// 読み込み中フラグ
		bool					isLoading = false;
	};

public:

	EnemyDataManager() = default;
	~EnemyDataManager() override {}

	// 初期化処理
	bool Initialize() override;
	// 名前取得
	std::string GetName() const override { return "EnemyDataManager"; }
	// ファイルパス取得
	std::string GetFilePath() const override { return _filePath; };
	// ファイル読み込み
	bool LoadFromFile() override;
	// ファイル保存
	bool SaveToFile() override;

	// Gui描画
	void DrawGui() override;


	// アクター登録
	template <typename T>
	void Register(const std::string& actorType, std::string modelFilePath)
	{
		auto& it = _enemyDatas[actorType];
		// ラムダ式で生成ロジックを保存
		it.makeFunc = []() -> std::shared_ptr<Actor>
			{
				return std::make_shared<T>();
			};
		it.modelFilePath = modelFilePath;
	}
	// アクター生成
	std::shared_ptr<Actor> CreateActor(Scene* scene,
		const std::string& actorType,
		const std::string& name);
	// モデル読み込み
	void LoadModel(const std::string& actorType);
	// モデル解放
	void ReleaseModel(const std::string& actorType);
	// モデルが読み込まれているか
	bool IsModelLoaded(const std::string& actorType);

	// 敵データマップ取得
	const std::unordered_map<std::string, EnemyData>& GetEnemyDataMap() const { return _enemyDatas; }

private:
	std::string _filePath = "./Data/Resource/EnemyDataManager.json";

	// 敵データマップ
	std::unordered_map<std::string, EnemyData> _enemyDatas;

	// 敵モデルマップ
	std::unordered_map<std::string, EnemyModelData> _enemyModels;

	// モデル読み込み用ミューテックス
	std::mutex _modelLoadMutex;
};

// リソース設定
_REGISTER_RESOURCE(EnemyDataManager)
