#include "ModelResource.h"

#include "../HRTrace.h"

// 埋め込みテクスチャを生成するためのやつ
#include <fstream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "SerializeFunction.h"

// Assimp To DirectX
namespace AssimpHelper
{
    static DirectX::XMFLOAT2 ToFloat2(const aiVector3D& v)
    {
        return DirectX::XMFLOAT2(
            static_cast<float>(v.x),
            static_cast<float>(v.y)
        );
    }
    static DirectX::XMFLOAT3 ToFloat3(const aiVector3D& v)
    {
        return DirectX::XMFLOAT3(
            static_cast<float>(v.x),
            static_cast<float>(v.y),
            static_cast<float>(v.z)
        );
    }
    static DirectX::XMFLOAT4 ToFloat4(const aiVector3D& v, float w)
    {
        return DirectX::XMFLOAT4(
            static_cast<float>(v.x),
            static_cast<float>(v.y),
            static_cast<float>(v.z),
            w
        );
    }
    static DirectX::XMFLOAT4 ToFloat4(const aiColor3D& v)
    {
        return DirectX::XMFLOAT4(
            static_cast<float>(v.r),
            static_cast<float>(v.g),
            static_cast<float>(v.b),
            1.0f
        );
    }
    static DirectX::XMFLOAT4 ToFloat4(const aiQuaternion& q)
    {
        return DirectX::XMFLOAT4(
            static_cast<float>(q.x),
            static_cast<float>(q.y),
            static_cast<float>(q.z),
            static_cast<float>(q.w)
        );
    }

    static DirectX::XMFLOAT4X4 ToFloat4X4(const aiMatrix4x4& m)
    {
        return DirectX::XMFLOAT4X4(
            static_cast<float>(m.a1),
            static_cast<float>(m.b1),
            static_cast<float>(m.c1),
            static_cast<float>(m.d1),
            static_cast<float>(m.a2),
            static_cast<float>(m.b2),
            static_cast<float>(m.c2),
            static_cast<float>(m.d2),
            static_cast<float>(m.a3),
            static_cast<float>(m.b3),
            static_cast<float>(m.c3),
            static_cast<float>(m.d3),
            static_cast<float>(m.a4),
            static_cast<float>(m.b4),
            static_cast<float>(m.c4),
            static_cast<float>(m.d4)
        );
    }
}

// モデルの読み込み
void ModelResource::Load(std::string filename)
{
    _filepath = filename;

    std::filesystem::path serializePath(filename);

    // 拡張子取得
    std::string extension = _filepath.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), tolower);

    // シリアライズの確認
    serializePath.replace_extension(MODEL_EXTENSION);
    _serializePath = serializePath.generic_string();
    if (std::filesystem::exists(serializePath))
    {
        // シリアライズされていることを保存
        _isSerialized = true;

        // シリアライズ読み込み
        Deserialize(serializePath.string().c_str());

        // ノードの構築
        BuildNode(_nodes);

        // ボーンの構築
        BuildBone(_meshes, _nodes);

        return;
    }
    else
    {
        // もしもfilenameの拡張子がMODEL_EXTENSIONで、ファイルが見つからなかった場合エラーを出す
        _ASSERT_EXPR_A(extension != MODEL_EXTENSION, "Serialize File not found!");
    }

    // シリアライズされていない
    _isSerialized = false;

    // FBXファイルの場合は特殊なインポートオプションを設定
    if (extension == ".fbx")
    {
        // $AssimpFBX$がついたノードの生成を抑制
        _aImporter.SetPropertyInteger(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    }

    // インポート時のフラグ
    uint32_t flag =
        aiProcess_MakeLeftHanded |          // 左手座標系化
        aiProcess_Triangulate |             // 三角形化
        aiProcess_JoinIdenticalVertices |   // 重複頂点のマージ
        aiProcess_PopulateArmatureData;     // ボーンの参照データを取得

    // ファイル読み込み
    _aScene = _aImporter.ReadFile(filename, flag);
    _ASSERT_EXPR_A(_aScene, _aImporter.GetErrorString());

    // ノードの読み込み
    LoadNodes(_nodes);

    // ノードの構築
    BuildNode(_nodes);

    // メッシュの読み込み
    LoadMeshes(_meshes, _nodes);

    // ボーンの構築
    BuildBone(_meshes, _nodes);

    // テクスチャの読み込み
    LoadMaterials(_materials);

    // アニメーションの読み込み
    LoadAnimations(_animations, _nodes);

    // シリアライズ
    Serialize(serializePath.string().c_str());
}

// アニメーションの追加
void ModelResource::AppendAnimations(std::string filename,
    std::string animationName)
{
    ModelResource modelResource;
    modelResource.Load(filename);
	AppendAnimations(modelResource.GetAnimations().at(0), modelResource.GetNodes(), animationName);
}

void ModelResource::AppendAnimations(const Animation& newAnimation,
    const std::vector<Node>& nodes,
    std::string animationName)
{
    // thisとanimationResourceのノードを一致させるマップ作成
    std::unordered_map<std::string, size_t> nodeMap;
    {
        size_t index = 0;
        for (auto& node : this->GetNodes())
        {
            nodeMap[node.name] = index;
            index++;
        }
    }

	// animationNameが空の場合は、newAnimationの名前を使用
    if (animationName.size() == 0)
    {
        animationName = newAnimation.name;
    }

    ModelResource::Animation animation{};
    animation.name = animationName;
    animation.secondsLength = newAnimation.secondsLength;
    animation.nodeAnims.resize(this->GetNodes().size());
    // thisと新規のモデルのノード割り当て処理
    for (auto& node : this->GetNodes())
    {
        size_t srcIndex = GetNodeIndex(this->GetNodes(), node.name.c_str());
        size_t dstIndex = GetNodeIndex(nodes, node.name.c_str());
        if (dstIndex != -1)
        {
            animation.nodeAnims[srcIndex] = newAnimation.nodeAnims[dstIndex];
        }
    }
    // アニメーションがなかったノードに対して初期姿勢を追加
    for (size_t nodeIndex = 0; nodeIndex < animation.nodeAnims.size(); ++nodeIndex)
    {
        const Node& node = this->GetNodes().at(nodeIndex);
        NodeAnim& nodeAnim = animation.nodeAnims.at(nodeIndex);

        // 移動
        if (nodeAnim.positionKeyframes.size() == 0)
        {
            VectorKeyframe& keyframe = nodeAnim.positionKeyframes.emplace_back();
            keyframe.seconds = 0.0f;
            keyframe.value = node.position;
        }
        if (nodeAnim.positionKeyframes.size() == 1)
        {
            VectorKeyframe& keyframe = nodeAnim.positionKeyframes.emplace_back();
            keyframe.seconds = animation.secondsLength;
            keyframe.value = nodeAnim.positionKeyframes.at(0).value;
        }
        // 回転
        if (nodeAnim.rotationKeyframes.size() == 0)
        {
            QuaternionKeyframe& keyframe = nodeAnim.rotationKeyframes.emplace_back();
            keyframe.seconds = 0.0f;
            keyframe.value = node.rotation;
        }
        if (nodeAnim.rotationKeyframes.size() == 1)
        {
            QuaternionKeyframe& keyframe = nodeAnim.rotationKeyframes.emplace_back();
            keyframe.seconds = animation.secondsLength;
            keyframe.value = nodeAnim.rotationKeyframes.at(0).value;
        }
        // スケール
        if (nodeAnim.scaleKeyframes.size() == 0)
        {
            VectorKeyframe& keyframe = nodeAnim.scaleKeyframes.emplace_back();
            keyframe.seconds = 0.0f;
            keyframe.value = node.scale;
        }
        if (nodeAnim.scaleKeyframes.size() == 1)
        {
            VectorKeyframe& keyframe = nodeAnim.scaleKeyframes.emplace_back();
            keyframe.seconds = animation.secondsLength;
            keyframe.value = nodeAnim.scaleKeyframes.at(0).value;
        }
    }

    this->_animations.push_back(animation);
}

// アニメーションの追加
void ModelResource::AppendAnimations(ModelResource* animationResource,
    std::string animationName)
{
    // thisとanimationResourceのノードを一致させるマップ作成
    std::unordered_map<std::string, size_t> nodeMap;
    {
        size_t index = 0;
        for (auto& node : this->GetNodes())
        {
            nodeMap[node.name] = index;
            index++;
        }
    }

    size_t animationIndex = 0;
    for (auto& animationData : animationResource->_animations)
    {
        ModelResource::Animation animation{};
        animation.name = animationName;
        if(animationIndex > 0)
            animation.name += std::to_string(animationIndex);
        animationIndex++;
        animation.secondsLength = animationData.secondsLength;
        animation.nodeAnims.resize(this->GetNodes().size());
        for (auto& node : this->GetNodes())
        {
            size_t srcIndex = GetNodeIndex(this->GetNodes(), node.name.c_str());
            size_t dstIndex = GetNodeIndex(animationResource->GetNodes(), node.name.c_str());
            if (dstIndex != -1)
            {
                animation.nodeAnims[srcIndex] = animationData.nodeAnims[dstIndex];
            }
        }
        // アニメーションがなかったノードに対して初期姿勢を追加
        for (size_t nodeIndex = 0; nodeIndex < animation.nodeAnims.size(); ++nodeIndex)
        {
            const Node& node = this->GetNodes().at(nodeIndex);
            NodeAnim& nodeAnim = animation.nodeAnims.at(nodeIndex);

            // 移動
            if (nodeAnim.positionKeyframes.size() == 0)
            {
                VectorKeyframe& keyframe = nodeAnim.positionKeyframes.emplace_back();
                keyframe.seconds = 0.0f;
                keyframe.value = node.position;
            }
            if (nodeAnim.positionKeyframes.size() == 1)
            {
                VectorKeyframe& keyframe = nodeAnim.positionKeyframes.emplace_back();
                keyframe.seconds = animation.secondsLength;
                keyframe.value = nodeAnim.positionKeyframes.at(0).value;
            }
            // 回転
            if (nodeAnim.rotationKeyframes.size() == 0)
            {
                QuaternionKeyframe& keyframe = nodeAnim.rotationKeyframes.emplace_back();
                keyframe.seconds = 0.0f;
                keyframe.value = node.rotation;
            }
            if (nodeAnim.rotationKeyframes.size() == 1)
            {
                QuaternionKeyframe& keyframe = nodeAnim.rotationKeyframes.emplace_back();
                keyframe.seconds = animation.secondsLength;
                keyframe.value = nodeAnim.rotationKeyframes.at(0).value;
            }
            // スケール
            if (nodeAnim.scaleKeyframes.size() == 0)
            {
                VectorKeyframe& keyframe = nodeAnim.scaleKeyframes.emplace_back();
                keyframe.seconds = 0.0f;
                keyframe.value = node.scale;
            }
            if (nodeAnim.scaleKeyframes.size() == 1)
            {
                VectorKeyframe& keyframe = nodeAnim.scaleKeyframes.emplace_back();
                keyframe.seconds = animation.secondsLength;
                keyframe.value = nodeAnim.scaleKeyframes.at(0).value;
            }
        }

        this->_animations.push_back(animation);
    }
}

// メッシュの変更
void ModelResource::ReplaceMeshes(const char* filename)
{
    // モデル読み込み
    ModelResource lodModel;
    lodModel.Load(filename);

	// メッシュ置き換え
	_meshes = lodModel.GetMeshes();
}

int ModelResource::FindNodeByName(std::string name)
{
    int i = 0;
    for (auto& node : _nodes)
    {
        if (node.name == name)
            return i;
        i++;
    }
    return -1;
}

#pragma region 読み込み
// ノードの読み込み
void ModelResource::LoadNodes(std::vector<Node>& nodes)
{
    // ルートノードから探査
    TraverseNode(nodes, _aScene->mRootNode, -1);
}

// メッシュの読み込み
void ModelResource::LoadMeshes(std::vector<Mesh>& meshes, std::vector<Node>& nodes)
{
    // nodesから探査
    TraverseMesh(meshes, nodes, _aScene->mRootNode);
}

// テクスチャの読み込み
void ModelResource::LoadMaterials(std::vector<Material>& materials)
{
    // ディレクトリパス取得
    std::filesystem::path dirpath(_filepath.parent_path());

    materials.resize(_aScene->mNumMaterials);
    for (uint32_t aMaterialIndex = 0; aMaterialIndex < _aScene->mNumMaterials; ++aMaterialIndex)
    {
        const aiMaterial* aMaterial = _aScene->mMaterials[aMaterialIndex];
        Material& material = materials.at(aMaterialIndex);

        // マテリアル名
        aiString aMaterialName;
        aMaterial->Get(AI_MATKEY_NAME, aMaterialName);
        material.name = aMaterialName.C_Str();

        aiColor3D aColor;
        // ディフューズ色
        if (AI_SUCCESS == aMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aColor))
        {
            material.colors["Diffuse"] = AssimpHelper::ToFloat4(aColor);
        }
        else
        {
            material.colors["Diffuse"] = DirectX::XMFLOAT4({ 1.0f,1.0f,1.0,1.0f });
        }
        // アンビエント色
        if (AI_SUCCESS == aMaterial->Get(AI_MATKEY_COLOR_AMBIENT, aColor))
        {
            material.colors["Ambient"] = AssimpHelper::ToFloat4(aColor);
        }
        else
        {
            material.colors["Ambient"] = DirectX::XMFLOAT4({ 1.0f,1.0f,1.0,1.0f });
        }
        // スペキュラー色
        if (AI_SUCCESS == aMaterial->Get(AI_MATKEY_COLOR_SPECULAR, aColor))
        {
            material.colors["Specular"] = AssimpHelper::ToFloat4(aColor);
        }
        else
        {
            material.colors["Specular"] = DirectX::XMFLOAT4({ 1.0f,1.0f,1.0,1.0f });
        }
        // ラフネス値
        float aFactor = 0.0f;
        if (AI_SUCCESS == aMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, aFactor))
        {
            material.colors["PBRFactor"].y = std::clamp(aFactor, 0.0f, 1.0f);
        }
        else
        {
            material.colors["PBRFactor"].y = 0.5f;
        }
        if (AI_SUCCESS == aMaterial->Get(AI_MATKEY_METALLIC_FACTOR, aFactor))
        {
            material.colors["PBRFactor"].z = std::clamp(aFactor, 0.0f, 1.0f);
        }
        else
        {
            material.colors["PBRFactor"].z = 0.0f;
        }

        // テクスチャ取得関数
        // dummyTextureValue : 16進数でABGRの順番
        auto LoadTexture = [&](aiTextureType aTextureType, Material::TextureData& textureData, DWORD dummyTextureValue, UINT dummyTextureDimension)
            {
                // テクスチャファイルパス取得
                aiString aTextureFilePath;
                if (AI_SUCCESS == aMaterial->GetTexture(aTextureType, 0, &aTextureFilePath))
                {
                    // テクスチャが埋め込みか確認
                    const aiTexture* aTexture = _aScene->GetEmbeddedTexture(aTextureFilePath.C_Str());
                    if (aTexture != nullptr)
                    {
                        // テクスチャファイルパス作成
                        std::filesystem::path textureFilePath(aTextureFilePath.C_Str());
                        if (textureFilePath == "*0")
                        {
                            // テクスチャファイル名がなければマテリアル名とテクスチャタイプから作成
                            textureFilePath = material.name + "_" + aiTextureTypeToString(aTextureType) +
                                "." + aTexture->achFormatHint;
                        }
                        textureFilePath = "Texture" / textureFilePath.filename();

                        // 埋め込みテクスチャを出力するディレクトリを確認
                        std::filesystem::path outputDirPath(dirpath / textureFilePath.parent_path());
                        if (!std::filesystem::exists(outputDirPath))
                        {
                            // なかったらディレクトリ作成
                            std::filesystem::create_directories(outputDirPath);
                        }

                        // 出力ディレクトリに画像ファイルを保存
                        std::filesystem::path outputFilePath(dirpath / textureFilePath);
                        if (!std::filesystem::exists(outputFilePath))
                        {
                            // 画像がなければ作成
                            // mHeight が0なら非圧縮の生データなのでそのままバイナリ出力
                            if (aTexture->mHeight == 0)
                            {
                                std::ofstream os(outputFilePath.string().c_str(), std::ios::binary);
                                os.write(reinterpret_cast<char*>(aTexture->pcData), aTexture->mWidth);
                            }
                            else
                            {
                                // リニアな画像データはpngで出力
                                outputFilePath.replace_extension(".png");
                                stbi_write_png(
                                    outputFilePath.string().c_str(),
                                    static_cast<int>(aTexture->mWidth),
                                    static_cast<int>(aTexture->mHeight),
                                    static_cast<int>(sizeof(uint32_t)),
                                    aTexture->pcData, 0);
                            }
                        }

                        // テクスチャファイルパスを格納
                        textureData.filename = textureFilePath.string();
                    }
                    else
                    {
                        // テクスチャファイルパスをそのまま格納
                        textureData.filename = aTextureFilePath.C_Str();
                    }
                }
                textureData.dummyTextureValue = dummyTextureValue;
                textureData.dummyTextureDimension = dummyTextureDimension;
            };

        // ディフューズテクスチャ取得
        LoadTexture(aiTextureType_DIFFUSE, material.textureDatas["Diffuse"], 0xFFFFFFFF, 16);
        // ノーマルテクスチャ取得
        LoadTexture(aiTextureType_NORMALS, material.textureDatas["Normal"], 0xFFFF7F7F, 16);
        // スペキュラー取得
        LoadTexture(aiTextureType_SPECULAR, material.textureDatas["Specular"], 0xFFFFFFFF, 16);
        // ラフネス取得
        LoadTexture(aiTextureType_SHININESS, material.textureDatas["Roughness"], 0xFF777777, 16);
        // エミッシブ取得
        LoadTexture(aiTextureType_EMISSIVE, material.textureDatas["Emissive"], 0xFF000000, 16);
    }
}

void ModelResource::LoadAnimations(const aiScene* aScene,
    std::vector<Animation>& animations, const std::vector<Node>& nodes)
{
    for (uint32_t aAnimationIndex = 0; aAnimationIndex < aScene->mNumAnimations; ++aAnimationIndex)
    {
        const aiAnimation* aAnimation = aScene->mAnimations[aAnimationIndex];
        Animation& animation = animations.emplace_back();

        // アニメーション情報
        animation.name = aAnimation->mName.C_Str();
        animation.secondsLength = static_cast<float>(aAnimation->mDuration / aAnimation->mTicksPerSecond);

        // ノード毎のアニメーション
        animation.nodeAnims.resize(nodes.size());
        for (uint32_t aChannelIndex = 0; aChannelIndex < aAnimation->mNumChannels; ++aChannelIndex)
        {
            const aiNodeAnim* aNodeAnim = aAnimation->mChannels[aChannelIndex];
            int nodeIndex = GetNodeIndex(nodes, aNodeAnim->mNodeName.C_Str());
            if (nodeIndex < 0)continue;

            const Node& node = nodes.at(nodeIndex);
            NodeAnim& nodeAnim = animation.nodeAnims.at(nodeIndex);

            // 位置
            for (uint32_t aPositionIndex = 0; aPositionIndex < aNodeAnim->mNumPositionKeys; ++aPositionIndex)
            {
                const aiVectorKey& aKey = aNodeAnim->mPositionKeys[aPositionIndex];
                // 小数点が存在するフレームにゴミデータがあるらしいので除外
                if (fabs(std::round(aKey.mTime) - aKey.mTime) > 0.001)continue;

                VectorKeyframe& keyframe = nodeAnim.positionKeyframes.emplace_back();
                keyframe.seconds = static_cast<float>(aKey.mTime / aAnimation->mTicksPerSecond);
                keyframe.value = AssimpHelper::ToFloat3(aKey.mValue);
            }
            // 回転
            for (uint32_t aRotationIndex = 0; aRotationIndex < aNodeAnim->mNumRotationKeys; ++aRotationIndex)
            {
                const aiQuatKey& aKey = aNodeAnim->mRotationKeys[aRotationIndex];
                // 小数点が存在するフレームにゴミデータがあるらしいので除外
                if (fabs(std::round(aKey.mTime) - aKey.mTime) > 0.001)continue;

                QuaternionKeyframe& keyframe = nodeAnim.rotationKeyframes.emplace_back();
                keyframe.seconds = static_cast<float>(aKey.mTime / aAnimation->mTicksPerSecond);
                keyframe.value = AssimpHelper::ToFloat4(aKey.mValue);
            }
            // スケール
            for (uint32_t aScaleIndex = 0; aScaleIndex < aNodeAnim->mNumScalingKeys; ++aScaleIndex)
            {
                const aiVectorKey& aKey = aNodeAnim->mScalingKeys[aScaleIndex];
                // 小数点が存在するフレームにゴミデータがあるらしいので除外
                if (fabs(std::round(aKey.mTime) - aKey.mTime) > 0.001)continue;

                VectorKeyframe& keyframe = nodeAnim.scaleKeyframes.emplace_back();
                keyframe.seconds = static_cast<float>(aKey.mTime / aAnimation->mTicksPerSecond);
                keyframe.value = AssimpHelper::ToFloat3(aKey.mValue);
            }
        }

        // アニメーションがなかったノードに対して初期姿勢を追加
        for (size_t nodeIndex = 0; nodeIndex < animation.nodeAnims.size(); ++nodeIndex)
        {
            const Node& node = nodes.at(nodeIndex);
            NodeAnim& nodeAnim = animation.nodeAnims.at(nodeIndex);

            // 移動
            if (nodeAnim.positionKeyframes.size() == 0)
            {
                VectorKeyframe& keyframe = nodeAnim.positionKeyframes.emplace_back();
                keyframe.seconds = 0.0f;
                keyframe.value = node.position;
            }
            if (nodeAnim.positionKeyframes.size() == 1)
            {
                VectorKeyframe& keyframe = nodeAnim.positionKeyframes.emplace_back();
                keyframe.seconds = animation.secondsLength;
                keyframe.value = nodeAnim.positionKeyframes.at(0).value;
            }
            // 回転
            if (nodeAnim.rotationKeyframes.size() == 0)
            {
                QuaternionKeyframe& keyframe = nodeAnim.rotationKeyframes.emplace_back();
                keyframe.seconds = 0.0f;
                keyframe.value = node.rotation;
            }
            if (nodeAnim.rotationKeyframes.size() == 1)
            {
                QuaternionKeyframe& keyframe = nodeAnim.rotationKeyframes.emplace_back();
                keyframe.seconds = animation.secondsLength;
                keyframe.value = nodeAnim.rotationKeyframes.at(0).value;
            }
            // スケール
            if (nodeAnim.scaleKeyframes.size() == 0)
            {
                VectorKeyframe& keyframe = nodeAnim.scaleKeyframes.emplace_back();
                keyframe.seconds = 0.0f;
                keyframe.value = node.scale;
            }
            if (nodeAnim.scaleKeyframes.size() == 1)
            {
                VectorKeyframe& keyframe = nodeAnim.scaleKeyframes.emplace_back();
                keyframe.seconds = animation.secondsLength;
                keyframe.value = nodeAnim.scaleKeyframes.at(0).value;
            }
        }
    }
}

// アニメーションの読み込み
void ModelResource::LoadAnimations(std::vector<Animation>& animations, const std::vector<Node>& nodes)
{
    LoadAnimations(this->_aScene, animations, nodes);
}

// ノードの再帰読み込み
void ModelResource::TraverseNode(std::vector<Node>& nodes,
    const aiNode* aNode,
    int parentIndex)
{
    // aiNode* からNode のインデックを取得できるようにする
    std::map<const aiNode*, int>::iterator it = _nodeIndexMap.find(aNode);
    if (it == _nodeIndexMap.end())
    {
        // 過去に登録していなければ、登録
        _nodeIndexMap[aNode] = static_cast<int>(nodes.size());
    }

    // トランスフォームデータ取得
    aiVector3D aScale, aPosition;
    aiQuaternion aRotation;
    aNode->mTransformation.Decompose(aScale, aRotation, aPosition);

    // ノードデータ格納
    Node& node = nodes.emplace_back();
    node.name = aNode->mName.C_Str();
    node.parentIndex = parentIndex;
    node.position = AssimpHelper::ToFloat3(aPosition);
    node.rotation = AssimpHelper::ToFloat4(aRotation);
    node.scale = AssimpHelper::ToFloat3(aScale);

    parentIndex = static_cast<int>(nodes.size() - 1);

    // 子ノードの探査
    for (uint32_t aNodeIndex = 0; aNodeIndex < aNode->mNumChildren; ++aNodeIndex)
    {
        TraverseNode(nodes, aNode->mChildren[aNodeIndex], parentIndex);
    }
}

// メッシュの再帰読み込み
void ModelResource::TraverseMesh(std::vector<Mesh>& meshes,
    std::vector<Node>& nodes,
    const aiNode* aNode)
{
    // メッシュデータ取得
    for (uint32_t aMeshIndex = 0; aMeshIndex < aNode->mNumMeshes; ++aMeshIndex)
    {
        const aiMesh* aMesh = _aScene->mMeshes[aNode->mMeshes[aMeshIndex]];

        // メッシュデータ格納
        Mesh& mesh = meshes.emplace_back();

        // 頂点データ
        mesh.vertices.resize(aMesh->mNumVertices);
        for (uint32_t aVertexIndex = 0; aVertexIndex < aMesh->mNumVertices; ++aVertexIndex)
        {
            Vertex& vertex = mesh.vertices.at(aVertexIndex);
            // 位置
            if (aMesh->HasPositions())
            {
                vertex.position = AssimpHelper::ToFloat3(aMesh->mVertices[aVertexIndex]);
            }
            // UV座標
            if (aMesh->HasTextureCoords(0))
            {
                // Assimp側のUV座標の取り方が左下を原点としているので
                // 左上を原点に調整する
                vertex.texcoord = AssimpHelper::ToFloat2(aMesh->mTextureCoords[0][aVertexIndex]);
                vertex.texcoord.y = 1.0f - vertex.texcoord.y;
            }
            // 法線情報
            if (aMesh->HasNormals())
            {
                vertex.normal = AssimpHelper::ToFloat3(aMesh->mNormals[aVertexIndex]);
                DirectX::XMStoreFloat3(&vertex.normal,
                    DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&vertex.normal)));
            }
            if (aMesh->HasTangentsAndBitangents())
            {
                vertex.tangent = AssimpHelper::ToFloat4(aMesh->mTangents[aVertexIndex], 1.0f);
            }
            else
            {
                // モデルによっては接線情報がないので作る
                // topをちょっとだけZにずらすことでtop==Nになることを防ぐ
                DirectX::XMVECTOR top = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0.0f, 1.0f, 0.01f, 0.0f));
                DirectX::XMVECTOR N = DirectX::XMLoadFloat3(&vertex.normal);
                DirectX::XMFLOAT3 t{};
                DirectX::XMStoreFloat3(&t,
                    DirectX::XMVector3Normalize(DirectX::XMVector3Cross(N, top)));
                vertex.tangent.x = t.x;
                vertex.tangent.y = t.y;
                vertex.tangent.z = t.z;
                vertex.tangent.w = 1.0f;
            }
        }

        // スキニングデータ
        if (aMesh->mNumBones > 0)
        {
            // ボーン影響力データ
            struct BoneInfluence
            {
                uint32_t indices[4] = { 0,0,0,0 };
                float weights[4] = { 1.0f,0.0f,0.0f,0.0f };
                int useCount = 0;

                void Add(uint32_t index, float weight)
                {
                    if (useCount >= 4)return;
                    for (int i = 0; i < useCount; ++i)
                    {
                        if (indices[i] == index)
                        {
                            return;
                        }
                    }
                    indices[useCount] = index;
                    weights[useCount] = weight;
                    useCount++;
                }

                void Normalize()
                {
                    size_t indexCount = 0;
                    float totalWeight = 0.0f;
                    for (size_t i = 0; i < 4; ++i)
                    {
                        if (indices[i] != 0)
                        {
                            indexCount++;
                            totalWeight += weights[i];
                        }
                    }

                    for (size_t i = 0; i < indexCount; ++i)
                    {
                        weights[i] /= totalWeight;
                    }
                }
            };

            std::vector<BoneInfluence> boneInfluences;
            boneInfluences.resize(aMesh->mNumVertices);

            // メッシュに影響するボーンデータを収集
            for (uint32_t aBoneIndex = 0; aBoneIndex < aMesh->mNumBones; ++aBoneIndex)
            {
                const aiBone* aBone = aMesh->mBones[aBoneIndex];

                // 頂点影響力データを抽出
                for (uint32_t aWightIndex = 0; aWightIndex < aBone->mNumWeights; ++aWightIndex)
                {
                    const aiVertexWeight& aWeight = aBone->mWeights[aWightIndex];
                    BoneInfluence& boneInfluence = boneInfluences.at(aWeight.mVertexId);
                    boneInfluence.Add(aBoneIndex, aWeight.mWeight);
                }

                // ボーンデータ取得
                Bone& bone = mesh.bones.emplace_back();
                bone.nodeIndex = _nodeIndexMap[aBone->mNode];
                bone.offsetTransform = AssimpHelper::ToFloat4X4(aBone->mOffsetMatrix);
            }

            //for (auto& boneInfluence : boneInfluences)
            //{
            //    boneInfluence.Normalize();
            //}

            // 頂点影響力データを格納
            for (size_t vertexIndex = 0; vertexIndex < mesh.vertices.size(); ++vertexIndex)
            {
                Vertex& vertex = mesh.vertices.at(vertexIndex);
                BoneInfluence& boneInfluence = boneInfluences.at(vertexIndex);
                vertex.boneWeight.x = boneInfluence.weights[0];
                vertex.boneWeight.y = boneInfluence.weights[1];
                vertex.boneWeight.z = boneInfluence.weights[2];
                vertex.boneWeight.w = boneInfluence.weights[3];
                vertex.boneIndex.x = boneInfluence.indices[0];
                vertex.boneIndex.y = boneInfluence.indices[1];
                vertex.boneIndex.z = boneInfluence.indices[2];
                vertex.boneIndex.w = boneInfluence.indices[3];

                //float f = (vertex.boneWeight.x + vertex.boneWeight.y + vertex.boneWeight.z + vertex.boneWeight.w);
                //if (f > 1.0f + FLT_EPSILON)
                //{
                //    vertex.boneWeight.x = 1.0f;
                //    vertex.boneWeight.y = 0.0f;
                //    vertex.boneWeight.z = 0.0f;
                //    vertex.boneWeight.w = 0.0f;
                //}
            }
        }

        // インデックデータ
        // faceはポリゴンのことで1ポリゴンにつき頂点は3個なので * 3
        mesh.indices.resize(aMesh->mNumFaces * 3);
        for (uint32_t aFaceIndex = 0; aFaceIndex < aMesh->mNumFaces; ++aFaceIndex)
        {
            const aiFace& aFace = aMesh->mFaces[aFaceIndex];
            uint32_t index = aFaceIndex * 3;
            mesh.indices[index + 0] = aFace.mIndices[0];
            mesh.indices[index + 1] = aFace.mIndices[1];
            mesh.indices[index + 2] = aFace.mIndices[2];
        }

        // マテリアルの番号設定
        mesh.materialIndex = static_cast<int>(aMesh->mMaterialIndex);

        // ノードの番号設定
        mesh.nodeIndex = _nodeIndexMap[aNode];
    }

    // 子ノードの探査
    for (uint32_t aNodeIndex = 0; aNodeIndex < aNode->mNumChildren; ++aNodeIndex)
    {
        TraverseMesh(meshes, nodes, aNode->mChildren[aNodeIndex]);
    }
}
#pragma endregion

#pragma region 構築
// ノードの構築
void ModelResource::BuildNode(std::vector<Node>& nodes)
{
    for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
    {
        Node& node = nodes.at(nodeIndex);

        // 親子関係を構築
        node.parent = node.parentIndex >= 0 ? &nodes.at(node.parentIndex) : nullptr;
        if (node.parent != nullptr)
        {
            // 親の方に子供として登録
            node.parent->children.emplace_back(&node);
        }
    }
}

// ボーンの構築
void ModelResource::BuildBone(std::vector<Mesh>& meshes, std::vector<Node>& nodes)
{
    for (Mesh& mesh : meshes)
    {
        for (Bone& bone : mesh.bones)
        {
            // 参照ノード設定
            bone.node = &nodes.at(bone.nodeIndex);
        }
    }
}
#pragma endregion

#pragma region LOD
// 中間LODメッシュの読み込み
void ModelResource::AppendMiddleLODMeshes(const char* filename)
{
    // モデル読み込み
    ModelResource lodModel;
    lodModel.Load(filename);

    _middleLODMeshes = lodModel.GetMeshes();
    _middleLODFilepath = filename;
    _hasMiddleLODData = _middleLODMeshes.size() > 0;
}
// 低LODメッシュの読み込み
void ModelResource::AppendLowLODMeshes(const char* filename)
{
    // モデル読み込み
    ModelResource lodModel;
    lodModel.Load(filename);

    _lowLODMeshes = lodModel.GetMeshes();
    _lowLODFilepath = filename;
    _hasLowLODData = _lowLODMeshes.size() > 0;
}
// LODシリアライズ
void ModelResource::SerializeLOD(const char* filename)
{
    if (!_hasMiddleLODData || !_hasLowLODData) return;

    std::filesystem::path serializePath = std::filesystem::path(filename).replace_extension(MODEL_LOD_EXTENSION);
    std::ofstream ostream(serializePath.string().c_str(), std::ios::binary);
    if (ostream.is_open())
    {
        cereal::BinaryOutputArchive archive(ostream);

        try
        {
            archive(
                CEREAL_NVP(_middleLODMeshes),
                CEREAL_NVP(_middleLODFilepath),
                CEREAL_NVP(_lowLODMeshes),
                CEREAL_NVP(_lowLODFilepath)
            );
        }
        catch (...)
        {
            _ASSERT_EXPR_A(false, "Model LOD serialize failed");
        }
    }

}
// LODデシリアライズ
void ModelResource::DeserializeLOD(const char* filename)
{
    std::filesystem::path serializePath = std::filesystem::path(filename).replace_extension(MODEL_LOD_EXTENSION);
    std::ifstream istream(serializePath.string().c_str(), std::ios::binary);
    if (istream.is_open())
    {
        cereal::BinaryInputArchive archive(istream);

        try
        {
            archive(
                CEREAL_NVP(_middleLODMeshes),
                CEREAL_NVP(_middleLODFilepath),
                CEREAL_NVP(_lowLODMeshes),
                CEREAL_NVP(_lowLODFilepath)
            );

            _hasMiddleLODData = _middleLODMeshes.size() > 0;
            _hasLowLODData = _lowLODMeshes.size() > 0;
        }
        catch (...)
        {
            _ASSERT_EXPR_A(false, "Model LOD deserialize failed");
        }
    }
}
#pragma endregion

// ノードインデックス取得
int ModelResource::GetNodeIndex(const std::vector<Node>& nodes, const char* name)
{
    int index = 0;
    for (const Node& node : nodes)
    {
        if (node.name == name)
        {
            return index;
        }
        index++;
    }
    return -1;
}

// シリアライズ
void ModelResource::Serialize(const char* filename)
{
    std::ofstream ostream(filename, std::ios::binary);
    if (ostream.is_open())
    {
        cereal::BinaryOutputArchive archive(ostream);

        try
        {
            archive(
                CEREAL_NVP(_nodes),
                CEREAL_NVP(_meshes),
                CEREAL_NVP(_materials),
                CEREAL_NVP(_animations)
            );
        }
        catch (...)
        {
            _ASSERT_EXPR_A(false, "Model serialize failed");
        }
    }

    // LODシリアライズ
    SerializeLOD(filename);
}

// デシリアライズ
void ModelResource::Deserialize(const char* filename)
{
    std::ifstream istream(filename, std::ios::binary);
    if (istream.is_open())
    {
        cereal::BinaryInputArchive archive(istream);

        try
        {
            archive(
                CEREAL_NVP(_nodes),
                CEREAL_NVP(_meshes),
                CEREAL_NVP(_materials),
                CEREAL_NVP(_animations)
            );
        }
        catch (...)
        {
            _ASSERT_EXPR_A(false, "Model deserialize failed");
        }
    }
    else
    {
        _ASSERT_EXPR_A(false, "Model File not found");
    }

    // LODデシリアライズ
    DeserializeLOD(filename);
}

template<class T>
void ModelResource::Node::serialize(T& archive)
{
    archive(
        CEREAL_NVP(name),
        CEREAL_NVP(parentIndex),
        CEREAL_NVP(position),
        CEREAL_NVP(rotation),
        CEREAL_NVP(scale)
    );
}

template<class T>
void ModelResource::Vertex::serialize(T& archive)
{
    archive(
        CEREAL_NVP(position),
        CEREAL_NVP(boneWeight),
        CEREAL_NVP(boneIndex),
        CEREAL_NVP(texcoord),
        CEREAL_NVP(normal),
        CEREAL_NVP(tangent)
    );
}

template<class T>
inline void ModelResource::Material::TextureData::serialize(T& archive)
{
    archive(
        CEREAL_NVP(filename),
        CEREAL_NVP(dummyTextureValue),
        CEREAL_NVP(dummyTextureDimension)
    );
}


template<class T>
void ModelResource::Material::serialize(T& archive)
{
    archive(
        CEREAL_NVP(name),
        CEREAL_NVP(colors),
        CEREAL_NVP(textureDatas)
    );
}

template<class T>
void ModelResource::Bone::serialize(T& archive)
{
    archive(
        CEREAL_NVP(nodeIndex),
        CEREAL_NVP(offsetTransform)
    );
}

template<class T>
void ModelResource::Mesh::serialize(T& archive)
{
    archive(
        CEREAL_NVP(vertices),
        CEREAL_NVP(indices),
        CEREAL_NVP(materialIndex),
        CEREAL_NVP(nodeIndex),
        CEREAL_NVP(bones)
    );
}

template<class T>
void ModelResource::VectorKeyframe::serialize(T& archive)
{
    archive(
        CEREAL_NVP(seconds),
        CEREAL_NVP(value)
    );
}

template<class T>
void ModelResource::QuaternionKeyframe::serialize(T& archive)
{
    archive(
        CEREAL_NVP(seconds),
        CEREAL_NVP(value)
    );
}

template<class T>
void ModelResource::NodeAnim::serialize(T& archive)
{
    archive(
        CEREAL_NVP(positionKeyframes),
        CEREAL_NVP(rotationKeyframes),
        CEREAL_NVP(scaleKeyframes)
    );
}

template<class T>
void ModelResource::Animation::serialize(T& archive)
{
    archive(
        CEREAL_NVP(name),
        CEREAL_NVP(secondsLength),
        CEREAL_NVP(nodeAnims)
    );
}
