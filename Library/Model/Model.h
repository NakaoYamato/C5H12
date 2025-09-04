#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include <string>

#include "../Math/Vector.h"
#include "ModelResource.h"
#include "../Material/Material.h"

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
	void UpdateTransform(std::vector<ModelResource::Node>& poseNode, const DirectX::XMFLOAT4X4& world) const;
	// ノードのトランスフォーム更新処理
	// ※親が存在するノードのみ
	void UpdateNodeTransform(ModelResource::Node* node);
	// ノードのデバッグ表示
	void DebugDrawNode(Vector4 nodeColor = Vector4::Red);
	// GUiの表示
	void DrawGui();

#pragma region アクセサ
	// ノードの名前から番号を取得
	int GetNodeIndex(const std::string& str)
	{
		int i = 0;
		for (auto& node : _poseNodes)
		{
			if (node.name == str)
				return i;
			i++;
		}
		return -1;
	}

	ModelResource* GetResource() { return _resource.get(); }
	std::vector<ModelResource::Node>& GetPoseNodes() { return _poseNodes; }
	// マテリアルの取得
	std::vector<Material>& GetMaterials() { return _materialMap; }
	void SetPoseNodes(const std::vector<ModelResource::Node>& nodes) { this->_poseNodes = nodes; }

	const char* GetFilename()const { return _filename.c_str(); }
#pragma endregion

	// 再シリアライズ
	void ReSerialize();

protected:
	/// <summary>
	/// COMオブジェクト生成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="fbx_filename"></param>
	void CreateComObject(ID3D11Device* device, const char* fbxFilename);

protected:
	// ファイルパス
	std::string _filename;
	// モデルデータ
	std::shared_ptr<ModelResource> _resource;
	// 姿勢用ノード
	std::vector<ModelResource::Node> _poseNodes;
	// マテリアル
	std::vector<Material> _materialMap;
	// デバッグ用
	int _debugNodeIndex = -1;
};