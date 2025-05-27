#pragma once

#include "Component.h"
#include "../../Library/Model/Model.h"
#include "../../Library/Model/AnimationEvent.h"

class Animator : public Component
{
public:
	/// <summary>
	/// ルートモーションのオプション
	/// </summary>
	enum class RootMotionOption
	{
		None,
		RemovePositionX,
		RemovePositionY,
		RemovePositionZ,
		RemovePositionXY,
		RemovePositionXZ,
		RemovePositionYZ,
		RemovePositionXYZ,
		UseOffset,
	};

public:
	Animator() {}
	~Animator()override {}

	// 名前取得
	const char* GetName()const { return "Animator"; }
	// 開始処理
	void Start()override;
	// 更新処理
	void Update(float elapsedTime) override;
	// デバッグ表示
	void DebugRender(const RenderContext& rc)override;
	// GUI描画
	void DrawGui() override;

public:
#pragma region アニメーション制御
	// アニメーション経過時間更新
	void UpdateAnimSeconds(float elapsedTime);

	// アニメーション再生
	void PlayAnimation(int index, bool loop, float blendSeconds = 0.0f);
	// アニメーション再生(名前から検索)
	void PlayAnimation(std::string name, bool loop, float blendSeconds = 0.0f);

	// アニメーション再生中か
	// index	: 指定のアニメーションが再生中か調べる-1ですべてから調べる
	bool IsPlayAnimation(int index = -1)const;

	// アニメーション計算処理
	void ComputeAnimation(int animationIndex, int nodeIndex, float time, ModelResource::Node& nodePose) const;
	// アニメーション計算処理
	void ComputeAnimation(int animationIndex, float time, std::vector<ModelResource::Node>& nodePoses) const;

	/// <summary>
	/// ブレンディング計算処理
	/// </summary>
	/// <param name="pose0">前のポーズ</param>
	/// <param name="pose1">次のポーズ</param>
	/// <param name="rate">ブレンド率</param>
	/// <param name="result">結果</param>
	void BlendPoseNode(
		const std::vector<ModelResource::Node>& pose0,
		const std::vector<ModelResource::Node>& pose1,
		float rate,
		std::vector<ModelResource::Node>& result)const;
#pragma endregion

#pragma region アニメーションイベント
	/// <summary>
	/// 再生中のアニメーションのイベントを取得
	/// </summary>
	/// <returns></returns>
	std::vector<AnimationEvent::EventData> GetCurrentEvents();

	void SetActiveAttackEvent(bool active) { _activeAttackEvent = active; }
#pragma endregion


#pragma region アクセサ
	/// <summary>
	/// 現在のアニメーション番号取得
	/// </summary>
	/// <returns></returns>
	int GetAnimationIndex() const { return _animationIndex; }
	/// <summary>
	/// アニメーション名から番号取得
	/// </summary>
	int GetAnimationIndex(const std::string& key) const;
	/// <summary>
	/// 現在のアニメーション経過時間取得
	/// </summary>
	/// <returns></returns>
	float GetAnimationTimer() const { return _animationTimer; }

	bool IsPlaying() const { return _isPlaying; }
	bool IsLoop() const { return _isLoop; }
	bool IsUseRootMotion() const { return _useRootMotion; }
	bool IsRemoveRootMovement() const { return _removeRootMovement; }
	bool IsRemoveRootRotation() const { return _removeRootRotation; }
	RootMotionOption GetRootMotionOption() const { return _rootMotionOption; }

	/// <summary>
	///	ルートモーションで使うノード番号設定
	/// </summary>
	/// <param name="index"></param>
	void SetRootNodeIndex(int index) { _rootNodeIndex = index; }
	/// <summary>
	/// ルートモーションで使うノード番号設定
	/// </summary>
	/// <param name="key"></param>
	void SetRootNodeIndex(const std::string& key);
	void SetRootMotionOption(RootMotionOption option) { _rootMotionOption = option; }
	void SetIsPlaying(bool isPlaying) { _isPlaying = isPlaying; }
	void SetIsLoop(bool isLoop) { _isLoop = isLoop; }
	void SetIsUseRootMotion(bool isUseRootMotion) { _useRootMotion = isUseRootMotion; }
	void SetIsRemoveRootMovement(bool isRemoveRootMovement) { _removeRootMovement = isRemoveRootMovement; }
	void SetIsRemoveRootRotation(bool isRemoveRootRotation) { _removeRootRotation = isRemoveRootRotation; }

	/// <summary>
	/// モデルをリセット
	/// </summary>
	/// <param name="model"></param>
	void ResetModel(std::shared_ptr<Model> model);

	/// <summary>
	/// アニメーション番号から名前取得
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	std::string GetAnimationName(int index)const;
	/// <summary>
	/// 現在のアニメーション名取得
	/// </summary>
	/// <returns></returns>
	std::string GetAnimationName() const;

	/// <summary>
	/// ルートモーションによる移動量取得
	/// </summary>
	/// <returns></returns>
	const Vector3& GetRootMovement() const { return _rootMovement; }
	/// <summary>
	///	アニメーションイベント取得
	/// </summary>
	/// <returns></returns>
	AnimationEvent& GetAnimationEvent() { return _animationEvent; }
#pragma endregion
private:
    /// <summary>
    /// ルートモーション計算
    /// </summary>
    /// <param name="elapsedTime"></param>
    /// <param name="poseNodes"></param>
    void CalcRootMotion(float elapsedTime, std::vector<ModelResource::Node>& poseNodes);

    /// <summary>
    /// アニメーションイベントの更新
    /// </summary>
    void UpdateAnimationEvent();

	/// <summary>
	/// アニメーションのデバッグ表示をフィルタ
	/// </summary>
	/// <param name="filterStr">これを含むアニメーションを表示</param>
	void Filtering(std::string filterStr);

private:
	// アニメーションするモデル
	std::weak_ptr<Model> _model;

#pragma region パラメータ
	int		_animationIndex = -1;
	int		_rootNodeIndex = -1;
	RootMotionOption _rootMotionOption = RootMotionOption::None;

	float	_animationTimer = 0.0f;
	float	_blendTimer = 0.0f;
	float	_blendEndTime = -1.0f;

	bool	_isPlaying		= false;
	bool	_isLoop			= false;
	bool	_useRootMotion	= false;
	bool	_removeRootMovement = false;
	bool	_removeRootRotation = false;

	Vector3	_rootOffset		= Vector3::Zero;
	Vector3	_rootMovement	= Vector3::Zero;
#pragma endregion

#pragma region イベント
	AnimationEvent _animationEvent{};

	// 攻撃イベントを発生させるか
	bool _activeAttackEvent = false;
#pragma endregion


#pragma region デバッグ用
	std::vector<int> _displayAnimationIndices;
	std::string _filterStr = "";
	std::vector<const char*> _nodeNames;

	float	_blendSeconds = 0.0f;
#pragma endregion
};