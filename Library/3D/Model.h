#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include <string>

#include "../Math/Vector.h"
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

	// ノードのデバッグ表示
	void DebugDrawNode(Vector4 nodeColor = Vector4::Red);

	// GUiの表示
	void DrawGui();

	// ノードの名前から番号を取得
	int GetNodeIndex(const std::string& str)
	{
		int i = 0;
		for (auto& node : _poseNode)
		{
			if (node.name == str)
				return i;
			i++;
		}
		return -1;
	}

	// ゲッター
	ModelResource* GetResource() { return _resource.get(); }
	std::vector<ModelResource::Node>& GetPoseNodes() { return _poseNode; }
	std::vector<ModelResource::Node>& GetAddressPoseNodes() { return _poseNode; }
	void SetPoseNodes(const std::vector<ModelResource::Node>& nodes) { this->_poseNode = nodes; }

	const char* GetFilepath()const { return _serializePath.c_str(); }

	// 再シリアライズ
	void ReSerialize();

protected:
	void CreateComObject(ID3D11Device* device, const char* fbx_filename);

protected:
	// ファイルパス
	std::string _serializePath;

	// モデルデータ
	std::shared_ptr<ModelResource> _resource;

	// 姿勢用ノード
	std::vector<ModelResource::Node> _poseNode;

	// デバッグ用
	int _debugNodeIndex = -1;
};