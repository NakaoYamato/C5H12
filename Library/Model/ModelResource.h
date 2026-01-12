#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <map>
#include <unordered_map>

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/// <summary>
/// 3Dモデルのリソース生成クラス
/// </summary>
class ModelResource
{
public:
	//-----------------------------------------------------------
	// メッシュデータ
	struct Node
	{
		std::string name{};
		int parentIndex = -1;
		DirectX::XMFLOAT3 position = {};
		DirectX::XMFLOAT4 rotation = {};
		DirectX::XMFLOAT3 scale = {};

		DirectX::XMFLOAT4X4 localTransform{};
		DirectX::XMFLOAT4X4 worldTransform{};

		Node* parent = nullptr;
		std::vector<Node*> children;

		// シリアライズ
		template<class T>
		void serialize(T& archive);
	};

	struct Vertex
	{
		DirectX::XMFLOAT3 position = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT4 boneWeight = { 1.0f,0.0f,0.0f,0.0f };
		DirectX::XMUINT4 boneIndex = { 0,0,0,0 };
		DirectX::XMFLOAT2 texcoord = { 0.0f,0.0f };
		DirectX::XMFLOAT3 normal = { 0.0f,1.0f,0.0f };
		DirectX::XMFLOAT4 tangent = { 0.0f,0.0f,0.0f,1.0f };

		// シリアライズ
		template<class T>
		void serialize(T& archive);
	};

	struct Material
	{
		std::string name;
		// 色情報
		// Key : Diffuse, Ambient, Specular, PBRFactor(g:Roughness, b:Metalness)
		std::unordered_map<std::string, DirectX::XMFLOAT4> colors;

		// テクスチャとそのSRVをまとめたデータ
		struct TextureData
		{
			std::string filename;
			DWORD dummyTextureValue = 0;
			UINT dummyTextureDimension = 0;
			// シリアライズ
			template<class T>
			void serialize(T& archive);
		};
		// Key	: Diffuse, Normal, Specular,Roughness,Emissive
		std::unordered_map<std::string, TextureData> textureDatas;

		// シリアライズ
		template<class T>
		void serialize(T& archive);
	};

	struct Bone
	{
		int nodeIndex{};
		DirectX::XMFLOAT4X4 offsetTransform{};

		Node* node = nullptr;

		// シリアライズ
		template<class T>
		void serialize(T& archive);
	};

	struct Mesh
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

		int materialIndex = 0;

		int nodeIndex = 0;

		std::vector<Bone> bones;

		// シリアライズ
		template<class T>
		void serialize(T& archive);
	};
	// メッシュデータ
	//-----------------------------------------------------------

public:
	//-----------------------------------------------------------
	// アニメーションデータ
	struct VectorKeyframe
	{
		float seconds{};
		DirectX::XMFLOAT3 value{};

		// シリアライズ
		template<class T>
		void serialize(T& archive);
	};

	struct QuaternionKeyframe
	{
		float seconds{};
		DirectX::XMFLOAT4 value{};

		// シリアライズ
		template<class T>
		void serialize(T& archive);
	};

	struct NodeAnim
	{
		std::vector<VectorKeyframe> positionKeyframes;
		std::vector<QuaternionKeyframe> rotationKeyframes;
		std::vector<VectorKeyframe> scaleKeyframes;

		// シリアライズ
		template<class T>
		void serialize(T& archive);
	};

	struct Animation
	{
		std::string name{};
		float secondsLength{};
		std::vector<NodeAnim> nodeAnims;

		// シリアライズ
		template<class T>
		void serialize(T& archive);
	};

	// アニメーションデータ
	//-----------------------------------------------------------

public:
	ModelResource() {}
	~ModelResource() {}

	// モデルの読み込み
	void Load(std::string filename);


	// アニメーションの追加
	void AppendAnimations(std::string filename,
		std::string animationName = "");
	// アニメーションの追加
	void AppendAnimations(const Animation& animation,
		const std::vector<Node>& nodes,
		std::string animationName = "");
	// アニメーションの追加
	void AppendAnimations(ModelResource* animationResource,
		std::string animationName = "");

#pragma region アクセサ
	const std::vector<Node>& GetNodes() const { return _nodes; }
	const std::vector<Mesh>& GetMeshes() const { return _meshes; }
	const std::vector<Material>& GetMaterials() const { return _materials; }
	const std::vector<Animation>& GetAnimations() const { return _animations; }
	const std::string& GetSerializePath() const { return _serializePath; }

	std::vector<Node>& GetAddressNodes() { return _nodes; }
	std::vector<Mesh>& GetAddressMeshes() { return _meshes; }
	std::vector<Material>& GetAddressMaterials() { return _materials; }
	std::vector<Animation>& GetAddressAnimations() { return _animations; }

	// 名前からノード番号を取得
	int FindNodeByName(std::string name);

	// 読み込んだデータがシリアライズされていたかどうか
	bool IsSerialized()const { return _isSerialized; }
#pragma endregion
	// シリアライズ
	void Serialize(const char* filename);

	// デシリアライズ
	void Deserialize(const char* filename);

#pragma region 読み込み
	// ノードの読み込み
	void LoadNodes(std::vector<Node>& nodes);

	// メッシュの読み込み
	void LoadMeshes(std::vector<Mesh>& meshes, std::vector<Node>& nodes);

	// テクスチャの読み込み
	void LoadMaterials(std::vector<Material>& materials);

	// アニメーションの読み込み
	static void LoadAnimations(const aiScene* _aScene, std::vector<Animation>& animations, const std::vector<Node>& nodes);
	void LoadAnimations(std::vector<Animation>& animations, const std::vector<Node>& nodes);

	// ノードの再帰読み込み
	void TraverseNode(std::vector<Node>& nodes, const aiNode* aNode, int parentIndex);

	// メッシュの再帰読み込み
	void TraverseMesh(std::vector<Mesh>& meshes, std::vector<Node>& nodes, const aiNode* aNode);
#pragma endregion

#pragma region 構築
	// ノードの構築
	void BuildNode(std::vector<Node>& nodes);

	// ボーンの構築
	void BuildBone(std::vector<Mesh>& meshes, std::vector<Node>& nodes);
#pragma endregion

#pragma region LOD
	// LOD用メッシュデータ取得
	const std::vector<Mesh>& GetMiddleLODMeshes() const { return _middleLODMeshes; }
	const std::vector<Mesh>& GetLowLODMeshes() const { return _lowLODMeshes; }
	// LOD用メッシュデータ取得（アドレス）
	std::vector<Mesh>& GetAddressMiddleLODMeshes() { return _middleLODMeshes; }
	std::vector<Mesh>& GetAddressLowLODMeshes() { return _lowLODMeshes; }
    // LOD用ファイルパス取得
    const std::string& GetMiddleLODFilepath() const { return _middleLODFilepath; }
    const std::string& GetLowLODFilepath() const { return _lowLODFilepath; }
    // LODデータの有無取得
    bool HasMiddleLODData() const { return _hasMiddleLODData; }
    bool HasLowLODData() const { return _hasLowLODData; }

    // 中間LODメッシュの読み込み
	void AppendMiddleLODMeshes(const char* filename);
    // 低LODメッシュの読み込み
    void AppendLowLODMeshes(const char* filename);


	// LODシリアライズ
	void SerializeLOD(const char* filename);

	// LODデシリアライズ
	void DeserializeLOD(const char* filename);
#pragma endregion


private:
	// ノードインデックス取得
	static int GetNodeIndex(const std::vector<Node>& nodes, const char* name);

private:
	std::filesystem::path _filepath;
	std::string _serializePath{};
	// 読み込んだデータがシリアライズされていたかどうか
	bool _isSerialized = false;

	Assimp::Importer _aImporter;
	const aiScene* _aScene = nullptr;

	std::vector<Node> _nodes;
	std::vector<Mesh> _meshes;
	std::vector<Material> _materials;
	std::vector<Animation> _animations;

	std::map<const aiNode*, int> _nodeIndexMap;

    // 中間LODメッシュデータ
    std::vector<Mesh> _middleLODMeshes;
    std::string _middleLODFilepath{};
    bool _hasMiddleLODData = false;

    // 低LODメッシュデータ
    std::vector<Mesh> _lowLODMeshes;
    std::string _lowLODFilepath{};
    bool _hasLowLODData = false;
};
