#pragma once

#include "Component.h"
#include "../../Library/3D/Model.h"

class Animator : public Component
{
public:
	Animator(Model* model);
	~Animator()override {}

	// 名前取得
	const char* GetName()const { return "Animator"; }

	// 開始処理
	void Start()override {}

	// 更新処理
	void Update(float elapsedTime) override;

	// GUI描画
	void DrawGui() override;

public:
	void SetModel(Model* model)
	{
		_model = model;

		// ノードキャッシュの生成
		_nodeCaches.resize(model->GetPoseNodes().size());
	}

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

	/// <summary>
	/// アニメーション名から番号取得
	/// </summary>
	int GetAnimationIndex(const std::string& key) const;

#pragma endregion
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

	Model* _model{};
};