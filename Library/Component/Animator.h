#pragma once

#include "Component.h"
#include "../../Library/Model/Model.h"
#include "../../Library/Model/AnimationEvent.h"
#include "../../Library/Model/AnimationCurve.h"

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

	/// <summary>
	/// 現在のアニメーションの回転量を取り除く
	/// </summary>
	/// <returns>回転変化量</returns>
	Quaternion RemoveRootRotation(int rootIndex);
#pragma endregion

#pragma region アニメーションイベント
	/// <summary>
	/// 再生中のアニメーションのイベントを取得
	/// </summary>
	/// <returns></returns>
	std::vector<AnimationEvent::EventData> GetCurrentEvents();
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
	/// <summary>
	/// アニメーションの終了時間取得
	/// </summary>
	/// <returns></returns>
	float GetAnimationEndTime() const
	{
		if (_animationIndex == -1) return 0.0f;
		return _model.lock()->GetResource()->GetAnimations().at(_animationIndex).secondsLength;
	}
	/// <summary>
	/// アニメーション速度取得
	/// </summary>
	/// <returns></returns>
	float GetAnimationSpeed() const { return _animationSpeed; }
	/// <summary>
	/// アニメーションが再生されているか
	/// </summary>
	/// <returns></returns>
	bool IsPlaying() const { return _isPlaying; }
	/// <summary>
	/// ループ再生するか
	/// </summary>
	/// <returns></returns>
	bool IsLoop() const { return _isLoop; }
	/// <summary>
	/// ポーズ中か
	/// </summary>
	/// <returns></returns>
	bool IsPaused() const { return _isPaused; }
	/// <summary>
	/// ブレンド処理をするか
	/// </summary>
	/// <returns></returns>
	bool IsBlending() const { return _isBlending; }
	/// <summary>
	/// ルートモーションを使うか
	/// </summary>
	/// <returns></returns>
	bool IsUseRootMotion() const { return _useRootMotion; }
	/// <summary>
	/// ルートモーション使用時にルートの移動量を取り除くか
	/// </summary>
	/// <returns></returns>
	bool IsRemoveRootMovement() const { return _removeRootMovement; }
	/// <summary>
	/// ルートモーション使用時にルートの回転量を取り除くか
	/// </summary>
	/// <returns></returns>
	bool IsRemoveRootRotation() const { return _removeRootRotation; }
	/// <summary>
	/// ルートモーションの設定取得
	/// </summary>
	/// <returns></returns>
	RootMotionOption GetRootMotionOption() const { return _rootMotionOption; }

	/// <summary>
	/// アニメーション速度設定
	/// </summary>
	/// <param name="speed"></param>
	void SetAnimationSpeed(float speed) { _animationSpeed = speed; }
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
	/// <summary>
	/// ルートモーションの設定
	/// </summary>
	/// <param name="option"></param>
	void SetRootMotionOption(RootMotionOption option) { _rootMotionOption = option; }
	/// <summary>
	/// ループ再生設定
	/// </summary>
	/// <param name="isLoop"></param>
	void SetIsLoop(bool isLoop) { _isLoop = isLoop; }
	/// <summary>
	/// ポーズ設定
	/// </summary>
	/// <param name="isPaused"></param>
	void SetIsPaused(bool isPaused) { _isPaused = isPaused; }
	/// <summary>
	/// ルートモーション使用設定
	/// </summary>
	/// <param name="isUseRootMotion"></param>
	void SetIsUseRootMotion(bool isUseRootMotion) { _useRootMotion = isUseRootMotion; }
	/// <summary>
	/// ルートモーションでルートの移動量を取り除くか設定
	/// </summary>
	/// <param name="isRemoveRootMovement"></param>
	void SetIsRemoveRootMovement(bool isRemoveRootMovement) { _removeRootMovement = isRemoveRootMovement; }
	/// <summary>
	/// ルートモーションでルートの回転量を取り除くか設定
	/// </summary>
	/// <param name="isRemoveRootRotation"></param>
	void SetIsRemoveRootRotation(bool isRemoveRootRotation) { _removeRootRotation = isRemoveRootRotation; }
	/// <summary>
	/// ルートモーションのオフセット設定
	/// </summary>
	/// <param name="offset"></param>
	void SetRootOffset(const Vector3& offset) { _rootOffset = offset; }

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

#pragma region デバッグ用
	void SetDrawEventDebugGui(bool draw) { _drawEventDebugGui = draw; }
	void SetDrawAnimationCurveGui(bool draw) { _drawAnimationCurveGui = draw; }
#pragma endregion

#pragma region 部分アニメーション
	/// <summary>
	/// 部分アニメーション再生中か
	/// </summary>
	/// <returns></returns>
	bool IsPartialAnimationPlaying() const { return _isPartialPlaying; }
	/// <summary>
	/// 部分アニメーションさせるノードを設定
	/// </summary>
	/// <param name="nodeName"></param>
	void SetPartialAnimationMask(const std::string& nodeName);
	/// <summary>
	/// 部分アニメーションさせるノードを解除
	/// </summary>
	/// <param name="nodeName"></param>
	void RemovePartialAnimationMask(const std::string& nodeName);

	/// <summary>
	/// 特定のノード以下に別のアニメーションを再生する（部分アニメーション）
	/// </summary>
	/// <param name="animationName">アニメーション名</param>
	/// <param name="loop">ループするか</param>
	/// <param name="blendSeconds">ブレンドにかける時間</param>
	void PlayPartialAnimation(std::string animationName, bool loop = false, float blendSeconds = 0.2f);

	/// <summary>
	/// 部分アニメーションを停止する
	/// </summary>
	/// <param name="blendSeconds">フェードアウトにかける時間</param>
	void StopPartialAnimation(float blendSeconds = 0.2f);
#pragma endregion

private:
    /// <summary>
    /// ルートモーション計算
    /// </summary>
    /// <param name="elapsedTime"></param>
    /// <param name="poseNodes"></param>
    void CalcRootMotion(float elapsedTime, std::vector<ModelResource::Node>& poseNodes);

	/// <summary>
	/// アニメーションのデバッグ表示をフィルタ
	/// </summary>
	/// <param name="filterStr">これを含むアニメーションを表示</param>
	void Filtering(std::string filterStr);

	/// <summary>
	/// 部分アニメーション更新処理
	/// </summary>
	/// <param name="elapsedTime"></param>
	void UpdatePartialAnimation(float elapsedTime);

private:
	// アニメーションするモデル
	std::weak_ptr<Model> _model;

#pragma region パラメータ
	int		_animationIndex = -1;
	int		_rootNodeIndex = -1;
	RootMotionOption _rootMotionOption = RootMotionOption::None;

	float	_animationTimer = 0.0f;
	float	_animationSpeed = 1.0f;
	float	_blendTimer = 0.0f;
	float	_blendEndTime = -1.0f;

	bool	_isPlaying		= false;
	bool	_isLoop			= false;
	bool	_isPaused		= false;
	bool	_isBlending		= false;
	bool	_useRootMotion	= false;
	bool	_removeRootMovement = false;
	bool	_removeRootRotation = false;

	Vector3	_rootOffset		= Vector3::Zero;
	Vector3	_rootMovement	= Vector3::Zero;

	// アニメーションの最初の姿勢
	ModelResource::Node _startRootNode{};
#pragma endregion

#pragma region イベント
	AnimationEvent _animationEvent{};
#pragma endregion

#pragma region 再生速度カーブ
	AnimationCurve _animationCurve;
#pragma endregion


#pragma region デバッグ用
	std::vector<int> _displayAnimationIndices;
	std::string _filterStr = "";
	std::vector<const char*> _nodeNames;

	float	_blendSeconds = 0.0f;
	bool	_drawEventDebugGui = false;
	bool	_drawAnimationCurveGui = false;
#pragma endregion

#pragma region 部分アニメーション
	// 部分アニメーションの状態
	enum class PartialState
	{
		None,       // なし
		FadingIn,   // フェードイン中
		Active,     // 再生中（ウェイト1.0）
		FadingOut   // フェードアウト中
	};
	// 部分アニメーション制御用パラメーター
	bool _isPartialPlaying = false;
	int _partialAnimationIndex = -1;
	float _partialAnimationTimer = 0.0f;
	bool _isPartialLoop = false;

	// ブレンド制御
	PartialState _partialState = PartialState::None;
	float _partialBlendTimer = 0.0f;    // 現在のブレンド経過時間
	float _partialBlendDuration = 0.0f; // ブレンドにかかる時間
	float _partialWeight = 0.0f;        // 現在の適用率 (0.0 ~ 1.0)

	// マスク用インデックス
	std::vector<int> _partialMaskIndices;

	// ルートモーション処理を行わない回数
    int _partialRootMotionIgnoreCount = 0;
#pragma endregion
};