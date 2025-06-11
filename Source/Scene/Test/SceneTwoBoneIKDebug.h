#pragma once

#include "../../Library/Scene/SceneManager.h"

class SceneTwoBoneIKDebug : public Scene
{
public:
	SceneTwoBoneIKDebug() {}
	~SceneTwoBoneIKDebug()override {}

	// 名前取得
	const char* GetName()const { return u8"TwoBoneIKDebug"; }
	// 階層取得
	SceneMenuLevel GetLevel() const { return SceneMenuLevel::Debug; }

	//初期化
	void Initialize()override;
	// 更新処理
	void Update(float elapsedTime)override;

	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneTwoBoneIKDebug>();
	}
private:
	struct Bone
	{
		Bone* parent;
		Bone* child;
		DirectX::XMFLOAT3	localPosition;
		DirectX::XMFLOAT4	localRotation;
		DirectX::XMFLOAT4X4	worldTransform;

		// ワールド行列更新処理
		void UpdateWorldTransform()
		{
			DirectX::XMMATRIX LocalRotationTransform = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&localRotation));
			DirectX::XMMATRIX LocalPositionTransform = DirectX::XMMatrixTranslation(localPosition.x, localPosition.y, localPosition.z);
			DirectX::XMMATRIX LocalTransform = DirectX::XMMatrixMultiply(LocalRotationTransform, LocalPositionTransform);
			if (parent != nullptr)
			{
				DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&parent->worldTransform);
				DirectX::XMMATRIX WorldTransform = DirectX::XMMatrixMultiply(LocalTransform, ParentWorldTransform);
				DirectX::XMStoreFloat4x4(&worldTransform, WorldTransform);
			}
			else
			{
				DirectX::XMStoreFloat4x4(&worldTransform, LocalTransform);
			}
		}
		// 自身以下のワールド行列更新処理
		void UpdateWorldTransforms()
		{
			Bone* bone = this;
			while (bone != nullptr)
			{
				bone->UpdateWorldTransform();
				bone = bone->child;
			}
		}
	};

	Bone								bones[4];
	DirectX::XMFLOAT4X4					targetWorldTransform{};
	DirectX::XMFLOAT4X4					poleLocalTransform{};
	DirectX::XMFLOAT4X4					poleWorldTransform{};
};

// メニューバーに登録
_REGISTER_SCENE(SceneTwoBoneIKDebug)