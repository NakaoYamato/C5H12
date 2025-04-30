#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "../../Library/Math/Vector.h"

class AnimationCollisionData
{
public:
	/// <summary>
	/// 判定の種類
	/// </summary>
	enum CollisionType
	{
		// 当たり判定
		Hit,
		// 攻撃判定
		Attack,
		// その他判定
		Other,

		CollisionTypeEnd
	};

	/// <summary>
	/// 判定の形状
	/// </summary>
	enum ShapeType
	{
		Box,
		Sphere,
		Capsule,

		ShapeTypeEnd
	};

	struct Keyframe
	{
		CollisionType collisionType		= CollisionType::Hit;
		ShapeType shapeType				= ShapeType::Box;

		int nodeIndex				= 0;
		std::string	triggerMessage	= "";
		float startSeconds			= 0.0f;
		float endSeconds			= 0.0f;

		Vector3 position			= _VECTOR3_ZERO;
		Vector3 angle				= _VECTOR3_ZERO;
		Vector3 scale				= _VECTOR3_ONE;

		void DrawGui(const std::vector<const char*>& nodeNames);

		// シリアライズ
		template<class T>
		void serialize(T& archive);
	};
	using Keyframes = std::vector<Keyframe>;
public:
	AnimationCollisionData() {}
	~AnimationCollisionData() {}

	// すべてのGUI描画
	void DrawGuiAll(const std::vector<const char*>& nodeNames);
	// 指定したKeyframesのGUI描画
	void DrawGui(const std::string& animName, const std::vector<const char*>& nodeNames);

	// 要素を追加
	void AddKeyFrames(const std::string& animName)
	{
		_data[animName];
	}

	// 要素全削除
	void Clear()
	{
		_data.clear();
	}

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
#pragma region アクセサ
	Keyframes& GetKeyframes(const std::string& animName) {
		return _data[animName];
	}
#pragma endregion
private:
	// key		: アニメーション名
	// Value	: キーデータ 
	std::unordered_map<std::string, Keyframes> _data;
};
