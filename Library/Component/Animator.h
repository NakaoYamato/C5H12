#pragma once

#include "Component.h"
#include "../../Library/Model/Model.h"

class Animator : public Component
{
public:
	Animator() {}
	~Animator()override {}

	// 名前取得
	const char* GetName()const { return "Animator"; }
	// 開始処理
	void Start()override;
	// 更新処理
	void Update(float elapsedTime) override;
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
	void ComputeAnimation(int animationIndex, float time, std::vector<ModelResource::Node>& nodePoses) const;

	// ブレンディング計算処理
	std::vector<ModelResource::Node> ComputeBlending(
		const std::vector<ModelResource::Node>& pose0,
		const std::vector<ModelResource::Node>& pose1, float rate) const;

	/// <summary>
	/// ルートモーション処理
	/// </summary>
	/// <param name="animationIndex">アニメーション番号</param>
	/// <param name="oldAnimSeconds">前フレームのアニメーション経過時間</param>
	/// <param name="currentAnimSeconds">今フレームのアニメーション経過時間</param>
	/// <param name="controlNodeIndex">ルートモーションを適応させるノード</param>
	/// <param name="resultNodePose">処理を行った後のノード</param>
	/// <param name="movement">モデル空間内での移動量</param>
	void ComputeRootMotion(int animationIndex,
		float oldAnimSeconds, float currentAnimSeconds,
		int controlNodeIndex,
		std::vector<ModelResource::Node>& resultNodePose,
		Vector3& movement) const;

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
	bool IsBlending() const { return _isBlending; }
	bool IsUseRootMotion() const { return _useRootMotion; }
	bool IsRemoveMovement() const { return _removeMovement; }

	/// <summary>
	///	ルートモーションで使うノード番号設定
	/// </summary>
	/// <param name="index"></param>
	void SetRootNodeIndex(int index) { _rootNodeIndex = index; }
	/// <summary>
	/// ルートモーションで使うノード番号設定
	/// </summary>
	/// <param name="key"></param>
	void SetRootNodeIndex(const std::string& key) { _rootNodeIndex = GetAnimationIndex(key); }
	/// <summary>
	/// 移動量を取り除くノード番号設定
	/// </summary>
	/// <param name="index"></param>
	void SetRemoveMovementNodeIndex(int index) { _removeMovementNodeIndex = index; }
	/// <summary>
	/// 移動量を取り除くノード番号設定
	/// </summary>
	/// <param name="key"></param>
	void SetRemoveMovementNodeIndex(const std::string& key) { _removeMovementNodeIndex = GetAnimationIndex(key); }
	void SetIsPlaying(bool isPlaying) { _isPlaying = isPlaying; }
	void SetIsLoop(bool isLoop) { _isLoop = isLoop; }
	void SetIsBlending(bool isBlending) { _isBlending = isBlending; }
	void SetIsUseRootMotion(bool isUseRootMotion) { _useRootMotion = isUseRootMotion; }
	void SetIsRemoveMovement(bool isRemoveMovement) { _removeMovement = isRemoveMovement; }

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

	const Vector3& GetRootMovement() const { return _rootMovement; }
#pragma endregion
private:
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
	int 	_removeMovementNodeIndex = -1;

	float	_animationTimer = 0.0f;
	float	_blendTimer = 0.0f;
	float	_blendEndTime = -1.0f;

	bool	_isPlaying		= false;
	bool	_isLoop			= false;
	bool	_isBlending		= false;
	bool	_useRootMotion	= false;
	bool	_removeMovement = false;

	std::vector<ModelResource::Node> _cacheNodes;
	Vector3 _rootOffset = Vector3::Zero;
	Vector3 _rootMovement = Vector3::Zero;
	Quaternion _rootRotation = Quaternion::Identity;
#pragma endregion

#pragma region デバッグ用
	std::vector<int> _displayAnimationIndices;
	std::string _filterStr = "";
	std::vector<const char*> _nodeNames;

	float	_blendSeconds = 0.0f;
#pragma endregion
};