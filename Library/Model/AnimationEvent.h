#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "../../Library/Math/Vector.h"
#include "Model.h"
#include "ModelCollision.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <ImSequencer.h>

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
		int messageIndex = -1;
		float startSeconds = 0.0f;
		float endSeconds = 0.0f;

		// 判定の位置 ShapeType::Capsule時はstart
		Vector3 position = Vector3::Zero;
		// 判定の回転 ShapeType::Capsule時はend
		Vector3 angle = Vector3::Zero;
		Vector3 scale = Vector3::One;

		void DrawGui(const std::vector<const char*>& messageList, const std::vector<const char*>& nodeNames, bool canEdit);

		// シリアライズ
		template<class T>
		void serialize(T& archive, const std::uint32_t version);
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
	void DebugRender(const std::string& animName, float animElapsedTime, const DirectX::XMFLOAT4X4& world);
	/// <summary>
	/// GUI描画
	/// </summary>
	/// <param name="canEdit">編集可能か</param>
	void DrawGui(bool canEdit = true);
	// 指定したEventDataのGUI描画
	void DrawGui(const std::string& animName, bool canEdit = true);

	void DrawGui(const std::string& animName, float currentAnimTime, bool canEdit);

	/// <summary>
	/// メッセージリストの編集GUI描画
	/// </summary>
	/// <param name="canEdit">編集可能か</param>
	void DrawMassageListGui(bool canEdit = true);
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
	/// <summary>
	/// アニメーションに含まれるデータマップを取得
	/// </summary>
	/// <param name="animName">アニメーション名</param>
	/// <returns></returns>
	EventDataMap& GetEventData(const std::string& animName) {
		return _data[animName];
	}

	/// <summary>
	/// 要素を追加
	/// </summary>
	/// <param name="animName"></param>
	void AddEventData(const std::string& animName)
	{
		_data[animName];
	}

	/// <summary>
	/// 要素全削除
	/// </summary>
	void Clear()
	{
		_data.clear();
	}

	/// <summary>
	/// アニメーション名と経過時間からイベントデータを取得
	/// </summary>
	/// <param name="animName">アニメーション名</param>
	/// <param name="animElapsedTime">アニメーション経過時間</param>
	/// <returns></returns>
	EventDataMap GetCurrentEventData(const std::string& animName, float animElapsedTime);

	/// <summary>
	/// メッセージリストを取得
	/// </summary>
	/// <returns></returns>
	const std::vector<std::string>& GetMessageList() const
	{
		return _messageList;
	}
#pragma endregion
private:
	std::weak_ptr<Model> _model;

	// メッセージリスト
	std::vector<std::string> _messageList;
	// key		: アニメーション名
	// Value	: イベントデータ 
	std::unordered_map<std::string, EventDataMap> _data;

	// モデルのノード名
	std::vector<const char*> _nodeNames;

private:
	// ImSequencer::SequenceInterface の実装
	class AnimationSequencer : public ImSequencer::SequenceInterface
	{
	public:
		AnimationSequencer(EventDataMap& data,
			const std::vector<std::string>& messageList,
			const std::vector<const char*>& nodeNames,
			bool canEdit,
			int* selectedEntry,
			std::vector<int>* startFrames, 
			std::vector<int>* endFrames);
		
		// ImSequencer::SequenceInterface の仮想関数
		virtual int GetFrameMin() const override { return 0; }
		// フレーム数を適切な値に設定する必要があります。ここでは仮に1000としていますが、
		// 実際はアニメーションの長さ（秒数）とフレームレートから計算すべきです。
		virtual int GetFrameMax() const override { return 1000; }
		virtual int GetItemCount() const override { return (int)m_data.size(); }

		virtual void Get(int index, int** start, int** end, int* type, unsigned int* color) override;

		virtual const char* GetItemLabel(int index) const override;
		virtual int GetItemTypeCount() const override;
		virtual const char* GetItemTypeName(int typeIndex) const override;
	private:
		EventDataMap& m_data; // 参照としてイベントデータを持つ
		const std::vector<std::string>& m_messageList;
		const std::vector<const char*>& m_nodeNames;
		bool m_canEdit;
		int* m_selectedEntry;
		// *** 追加: DrawGuiから渡されるフレームバッファへのポインター ***
		std::vector<int>* m_startFrames;
		std::vector<int>* m_endFrames;
		// イベントタイプ名と色
		static const char* EventTypeNames[];
		static unsigned int EventTypeColors[];
	};
};

