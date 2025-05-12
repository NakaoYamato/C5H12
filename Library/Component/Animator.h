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
	// アニメーション更新処理
	void UpdateAnimation(float elapsedTime);

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
	void ResetModel(std::shared_ptr<Model> model);

	int GetCurrentAnimIndex()const { return _currentAnimIndex; }
	float GetCurrentAnimSeconds() const { return _currentAnimSeconds; }

	/// <summary>
	/// アニメーション名から番号取得
	/// </summary>
	int GetAnimationIndex(const std::string& key) const;

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
	std::string GetCurrentAnimationName() const;
#pragma endregion
private:
	/// <summary>
	/// アニメーションのデバッグ表示をフィルタ
	/// </summary>
	/// <param name="filterStr">これを含むアニメーションを表示</param>
	void Filtering(std::string filterStr);

public:
	// アニメーションパラメーター
	int _currentAnimIndex = -1;
	float _currentAnimSeconds = 0.0f;
	bool _animPlaying = false;
	bool _animLoop = false;

	// ブレンドアニメーションパラメーター
	std::vector<ModelResource::Node> _nodeCaches;
	float _animBlendSeconds = 0.0f;
	float _currentAnimBlendSeconds = 0.0f;
	float _animBlendSecondsLength = -1.0f;
	bool _animBlending = false;

private:
	std::weak_ptr<Model> _model;

	// デバッグ表示用
	std::vector<int> _displayAnimationIndices;
	std::string filterStr = "";
};