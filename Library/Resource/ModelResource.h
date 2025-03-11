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
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
			DWORD dummyTextureValue;
			UINT dummyTextureDimension;
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
		Node* node = nullptr;

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
	void AppendAnimations(ModelResource* animationResource);

	// アクセサ
	const std::vector<Node>& GetNodes() const { return nodes_; }
	const std::vector<Mesh>& GetMeshes() const { return meshes_; }
	const std::vector<Material>& GetMaterials() const { return materials_; }
	const std::vector<Animation>& GetAnimations() const { return animations_; }
	const std::string& GetSerializePath() const { return serializePath_; }

	std::vector<Mesh>& GetAddressMeshes() { return meshes_; }
	std::vector<Material>& GetAddressMaterials() { return materials_; }

	// 指定のマテリアルのSRVを変更
	void ChangeMaterialSRV(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv, int materialIndex, std::string textureKey)
	{
		materials_.at(materialIndex).textureDatas.at(textureKey).textureSRV = srv;
	}

	// シリアライズ
	void Serialize(const char* filename);

	// デシリアライズ
	void Deserialize(const char* filename);

private:
	// ノードの読み込み
	void LoadNodes(std::vector<Node>& nodes);

	// メッシュの読み込み
	void LoadMeshes(std::vector<Mesh>& meshes, std::vector<Node>& nodes);

	// テクスチャの読み込み
	void LoadMaterials(std::vector<Material>& materials);

	// アニメーションの読み込み
	void LoadAnimations(std::vector<Animation>& animations, const std::vector<Node>& nodes);

private:
	// ノードの再帰読み込み
	void TraverseNode(std::vector<Node>& nodes, const aiNode* aNode, int parentIndex);

	// メッシュの再帰読み込み
	void TraverseMesh(std::vector<Mesh>& meshes, std::vector<Node>& nodes, const aiNode* aNode);

	// ノードの構築
	void BuildNode(std::vector<Node>& nodes);

	// ボーンの構築
	void BuildBone(std::vector<Mesh>& meshes, std::vector<Node>& nodes);

private:
	// ノードインデックス取得
	static int GetNodeIndex(const std::vector<Node>& nodes, const char* name);

private:
	std::filesystem::path filepath_;
	std::string serializePath_{};

	Assimp::Importer aImporter_;
	const aiScene* aScene_ = nullptr;

	std::vector<Node> nodes_;
	std::vector<Mesh> meshes_;
	std::vector<Material> materials_;
	std::vector<Animation> animations_;

	std::map<const aiNode*, int> nodeIndexMap_;
};
