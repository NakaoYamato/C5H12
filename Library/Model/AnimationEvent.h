#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "../../Library/Math/Vector.h"
#include "Model.h"

class AnimationEvent
{
public:
	/// <summary>
	/// イベントの種類
	/// </summary>
	enum class EventType
	{
		// イベント情報
		Flag,
		// 当たり判定
		Hit,
		// 攻撃判定
		Attack,

		EventTypeEnd
	};

	/// <summary>
	/// 判定の形状
	/// </summary>
	enum class ShapeType
	{
		Box,
		Sphere,
		Capsule,

		ShapeTypeEnd
	};

	struct EventData
	{
		EventType eventType = EventType::Hit;
		ShapeType shapeType = ShapeType::Box;

		int nodeIndex = 0;
		std::string	triggerMessage = "";
		float startSeconds = 0.0f;
		float endSeconds = 0.0f;

		// 判定の位置 ShapeType::Capsule時はstart
		Vector3 position = Vector3::Zero;
		// 判定の回転 ShapeType::Capsule時はend
		Vector3 angle = Vector3::Zero;
		Vector3 scale = Vector3::One;

		void DrawGui(const std::vector<const char*>& nodeNames, bool canEdit);

		// シリアライズ
		template<class T>
		void serialize(T& archive);
	};
	using EventDataMap = std::vector<EventData>;
public:
	AnimationEvent() = default;
	~AnimationEvent() = default;

	/// <summary>
	/// モデル情報読み込み
	/// </summary>
	/// <param name="modelResource"></param>
	void Load(std::weak_ptr<Model> model);

	/// <summary>
	/// デバッグ表示
	/// </summary>
	/// <param name="animName"></param>
	/// <param name="animElapsedTime"></param>
	void DebugRender(const std::string& animName, float animElapsedTime);

	/// <summary>
	/// GUI描画
	/// </summary>
	/// <param name="canEdit">編集可能か</param>
	void DrawGui(bool canEdit = true);
	// 指定したEventDataのGUI描画
	void DrawGui(const std::string& animName, bool canEdit = true);

#pragma region ファイル操作
	/// <summary>
	/// データ書き出し
	/// </summary>
	/// <param name="filename"></param>
	/// <returns>失敗したらfalse</returns>
	bool Serialize(const char* filename);

	/// <summary>
	/// データ読み込み
	/// </summary>
	/// <param name="filename"></param>
	/// <returns>失敗したらfalse</returns>
	bool Deserialize(const char* filename);
#pragma endregion

#pragma region アクセサ
	EventDataMap& GetEventData(const std::string& animName) {
		return _data[animName];
	}

	// 要素を追加
	void AddEventData(const std::string& animName)
	{
		_data[animName];
	}

	// 要素全削除
	void Clear()
	{
		_data.clear();
	}
#pragma endregion
private:
	std::weak_ptr<Model> _model;
	// key		: アニメーション名
	// Value	: イベントデータ 
	std::unordered_map<std::string, EventDataMap> _data;

	std::vector<const char*> _nodeNames;
};

