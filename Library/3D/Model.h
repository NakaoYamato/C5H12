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
		for (auto& node : copyNodes)
		{
			if (node.name == str)
				return i;
			i++;
		}
		return -1;
	}

	// ゲッター
	ModelResource* GetResource() { return resource.get(); }
	const std::vector<ModelResource::Mesh>& GetMesh() const { return resource->GetMeshes(); }
	const std::vector<ModelResource::Material>& GetMaterials() const { return resource->GetMaterials(); }
	const std::vector<ModelResource::Node>& GetNodes()const { return copyNodes; }

	const char* GetFilepath()const { return filepath.c_str(); }

	void SetNodes(const std::vector<ModelResource::Node>& nodes) { this->copyNodes = nodes; }
protected:
	void CreateComObject(ID3D11Device* device, const char* fbx_filename);

protected:
	// ファイルパス
	std::string filepath;

	// モデルデータ
	std::shared_ptr<ModelResource> resource;

	// コピーノードデータ
	std::vector<ModelResource::Node> copyNodes;

	// デバッグ用
	int debugNodeIndex = -1;
};