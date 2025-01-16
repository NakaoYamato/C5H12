#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include <string>

#include "../Resource/ModelResource.h"

/// <summary>
/// モデルクラス
/// </summary>
class Model
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="device"></param>
	/// <param name="filename">モデルのファイルパス</param>
	Model(ID3D11Device* device, const char* filename);
	Model() = delete;
	virtual ~Model() {}

	// トランスフォーム更新処理
	void UpdateTransform(const DirectX::XMFLOAT4X4& world);

	// GUiの表示
	void DrawGui();

	// ノードの名前から番号を取得
	int GetNodeIndex(const std::string& str)
	{
		int i = 0;
		for (auto& node : poseNode)
		{
			if (node.name == str)
				return i;
			i++;
		}
		return -1;
	}

	// ゲッター
	ModelResource* GetResource() { return resource.get(); }
	const std::vector<ModelResource::Node>& GetPoseNodes()const { return poseNode; }
	void SetPoseNodes(const std::vector<ModelResource::Node>& nodes) { this->poseNode = nodes; }

	const char* GetFilepath()const { return serializePath_.c_str(); }
protected:
	void CreateComObject(ID3D11Device* device, const char* fbx_filename);

protected:
	// ファイルパス
	std::string serializePath_;

	// モデルデータ
	std::shared_ptr<ModelResource> resource;

	// 姿勢用ノード
	std::vector<ModelResource::Node> poseNode;

	// デバッグ用
	int debugNodeIndex = -1;
};