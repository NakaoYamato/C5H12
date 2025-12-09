#pragma once

#include "../../Library/Resource/ResourceManager.h"
#include "Actor.h"

#include <unordered_map>

// 前方宣言
class Scene;

class ActorFactory : public ResourceBase
{
public:
	// クリエーター関数の型定義 (ID3D11Device* を受け取ってインスタンスを返す)
	using CreatorFunc = std::function<std::shared_ptr<Actor>()>;

public:
	ActorFactory() = default;
	~ActorFactory() override {}

	// 名前取得
	std::string GetName() const override { return "ActorFactory"; }
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
	static void Register(const std::string& typeName) {
		// ラムダ式で生成ロジックを保存
		_creators[typeName] = []() -> std::shared_ptr<Actor>
			{
				return std::make_shared<T>();
			};
	}

	// アクター生成
	std::shared_ptr<Actor> CreateActor(Scene* scene, 
		const std::string& actorType,
		const std::string& name,
		ActorTag tag);

	// 登録されているアクタータイプの取得
	std::vector<std::string> GetRegisteredActorTypes() const;

private:
	std::string _filePath = "./Data/Resource/SkillManager.json";

	static std::unordered_map<std::string, CreatorFunc> _creators;
};

// リソース設定
_REGISTER_RESOURCE(ActorFactory)

// ActorFactoryに登録するための構造体
template<class T>
struct ActorFactoryRegister
{
	ActorFactoryRegister(std::string actorType)
	{
		ActorFactory::Register<T>(actorType);
	}
};

// ActorFactoryに登録するためのマクロ
// type			: Actor継承クラス
// actorType	: 登録するアクタータイプ名
#define _REGISTER_ACTOR_FACTORY(type, actorType) \
static ActorFactoryRegister<type> type##register(actorType);
