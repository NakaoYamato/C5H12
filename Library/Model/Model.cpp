#include "Model.h"

#include <sstream>
#include <functional>
#include <filesystem>
#include <set>
#include <imgui.h>

#include "../HRTrace.h"
#include "ModelResourceManager.h"
#include "../Graphics/GpuResourceManager.h"
#include "../Math/Quaternion.h"
#include "../Math/Matrix.h"

#include "../DebugSupporter/DebugSupporter.h"

Model::Model(ID3D11Device* device, const char* filename)
{
    // リソース読み込み
    _resource = ModelResourceManager::Instance().LoadModelResource(filename);

    assert(_resource != nullptr);

    // シリアライズ用ファイルパス取得
    _filename = _resource->GetSerializePath();

    // ノードデータをコピー
    CopyNodes();

    Debug::Output::String(L"FBXモデルの読み込み成功\n");
    Debug::Output::String("\t");
    Debug::Output::String(filename);
    Debug::Output::String("\n");

    /// COMオブジェクト生成
    CreateComObject(device, filename);
}

// トランスフォーム更新処理
void Model::UpdateTransform(std::vector<ModelResource::Node>& poseNode, const DirectX::XMFLOAT4X4& world) const
{
    for (ModelResource::Node& node : poseNode)
    {
        // ローカル行列算出
        DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
        DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation));
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.position.x, node.position.y, node.position.z);

        DirectX::XMMATRIX LocalT = S * R * T;

        // ワールド行列算出
        DirectX::XMMATRIX ParentWorld{};
        if (node.parent != nullptr)
        {
            ParentWorld = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);
        }
        else
        {
            ParentWorld = DirectX::XMLoadFloat4x4(&world);
        }
        DirectX::XMMATRIX WorldT = LocalT * ParentWorld;

        // 計算結果を保存
        DirectX::XMStoreFloat4x4(&node.localTransform, LocalT);
        DirectX::XMStoreFloat4x4(&node.worldTransform, WorldT);
    }
}

// ノードのトランスフォーム更新処理
void Model::UpdateNodeTransform(ModelResource::Node* node)
{
    // ローカル行列算出
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node->scale.x, node->scale.y, node->scale.z);
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node->rotation));
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node->position.x, node->position.y, node->position.z);

    DirectX::XMMATRIX LocalT = S * R * T;

    // ワールド行列算出
    DirectX::XMMATRIX ParentWorld = DirectX::XMLoadFloat4x4(&node->parent->worldTransform);
    DirectX::XMMATRIX WorldT = LocalT * ParentWorld;

    // 計算結果を保存
    DirectX::XMStoreFloat4x4(&node->localTransform, LocalT);
    DirectX::XMStoreFloat4x4(&node->worldTransform, WorldT);

	// 子供のワールドトランスフォームを更新
	for (auto& child : node->children)
	{
		UpdateNodeTransform(child);
	}
}

// ノードのデバッグ表示
void Model::DebugDrawNode(Vector4 nodeColor)
{
    // ボーン表示
    DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    // 親から子どもにボーンをのばす
    for (auto& node : GetPoseNodes())
    {
        if (node.parent != nullptr)
        {
            Vector3 childWP = Vector3(node.worldTransform._41, node.worldTransform._42, node.worldTransform._43);
            Vector3 parentWP = Vector3(node.parent->worldTransform._41, node.parent->worldTransform._42, node.parent->worldTransform._43);
            float length = Vector3::Length(parentWP - childWP);
            if (length == 0.0f)
                continue;
            DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&parentWP), DirectX::XMLoadFloat3(&childWP), Up);
            DirectX::XMFLOAT4X4 world;
            DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixInverse(nullptr, View));
            Debug::Renderer::DrawBone(
                world,
                length,
                nodeColor
            );
        }
    }
}

void Model::DrawGui()
{
    if (ImGui::TreeNode(u8"モデル"))
    {
        if (ImGui::TreeNode(u8"ノード"))
        {
			ImGui::Text(u8"ノード数:%d", static_cast<int>(_poseNodes.size()));
            std::function<void(ModelResource::Node&)> NodeGui = [&](ModelResource::Node& node)
                {
                    if (ImGui::TreeNodeEx(&node, ImGuiTreeNodeFlags_DefaultOpen, node.name.c_str()))
                    {
                        // ダブルクリックで選択
                        if (ImGui::IsItemClicked())
                        {
                            _debugNodeIndex = GetNodeIndex(node.name);
                        }
                        ImGui::Separator();
                        ImGui::DragFloat3(u8"position", &node.position.x, 0.1f);
                        Vector3 degrees = Vector3::ToDegrees(Quaternion::ToRollPitchYaw(node.rotation));
                        if (ImGui::DragFloat3(u8"degrees", &degrees.x, 0.1f))
                        {
                            node.rotation = Quaternion::FromRollPitchYaw(Vector3::ToRadians(degrees));
                        }
                        ImGui::DragFloat3(u8"scale", &node.scale.x, 0.1f);
                        ImGui::Separator();

                        for (auto child : node.children)
                        {
                            ImGui::Separator();
                            NodeGui(*child);
                        }

                        ImGui::TreePop();
                    }
                };
            NodeGui(_poseNodes.at(0));
            ImGui::TreePop();
        }
        ImGui::TreePop();
        ImGui::InputText(u8"ファイルパス", &_filename);
        if (ImGui::Button(u8"シリアライズ"))
        {
            ReSerialize();
        }
    }

    if (_debugNodeIndex != -1 && _debugNodeIndex < _poseNodes.size())
    {
        DirectX::XMFLOAT4X4 worldTransform = _poseNodes[_debugNodeIndex].worldTransform;
        //worldTransform._11 = 1.0f;
        //worldTransform._22 = 1.0f;
        //worldTransform._33 = 1.0f;
        //Debug::Renderer::DrawAxis(worldTransform);

		Vector3 p = {
			worldTransform._41,
			worldTransform._42,
			worldTransform._43
		};
		Vector3 x = Vector3::TransformCoord(Vector3(Vector3::Right) * 10.0f, worldTransform);
		Vector3 y = Vector3::TransformCoord(Vector3(Vector3::Up) * 10.0f, worldTransform);
		Vector3 z = Vector3::TransformCoord(Vector3(Vector3::Front) * 10.0f, worldTransform);
        Debug::Renderer::AddVertex(p);
        Debug::Renderer::AddVertex(x);
        Debug::Renderer::AddVertex(p);
        Debug::Renderer::AddVertex(y);
        Debug::Renderer::AddVertex(p);
        Debug::Renderer::AddVertex(z);

    }
}

// トランスフォームを適応
void Model::ApplyPoseToResource(ID3D11Device* device)
{
    static DirectX::XMFLOAT4X4 boneTransforms[256];
    auto Skinning = [&](const Vector4& v, float* boneWeights, uint32_t* boneIndices)
        {
            DirectX::XMVECTOR Result = DirectX::XMVectorZero();
            DirectX::XMVECTOR V = DirectX::XMLoadFloat4(&v);
            for (int i = 0; i < 4; ++i)
            {
                float weight = boneWeights[i];
                if (weight == 0.0f)
                    continue;
                UINT index = boneIndices[i];
                DirectX::XMMATRIX Bone = DirectX::XMLoadFloat4x4(&boneTransforms[index]);
                DirectX::XMVECTOR Pos = DirectX::XMVector4Transform(V, Bone);
                Result = DirectX::XMVectorAdd(Result, DirectX::XMVectorScale(Pos, weight));
            }
            Vector4 result;
            DirectX::XMStoreFloat4(&result, Result);
            return result;
        };

    for (ModelResource::Mesh& mesh : _resource->GetAddressMeshes())
    {
		// ボーン行列計算
        if (mesh.bones.size() > 0)
        {
            for (size_t i = 0; i < mesh.bones.size(); ++i)
            {
                const ModelResource::Bone& bone = mesh.bones.at(i);
                DirectX::XMMATRIX World = DirectX::XMLoadFloat4x4(&_poseNodes[bone.nodeIndex].worldTransform);
                DirectX::XMMATRIX Offset = DirectX::XMLoadFloat4x4(&bone.offsetTransform);
                DirectX::XMMATRIX Bone = Offset * World;
                DirectX::XMStoreFloat4x4(&boneTransforms[i], Bone);
            }
        }
        else
        {
            boneTransforms[0] = _poseNodes[mesh.nodeIndex].worldTransform;
        }

        for (auto& vertex : mesh.vertices)
        {
			Vector4 v = { vertex.position.x, vertex.position.y, vertex.position.z, 1.0f };
            v = Skinning(v, &vertex.boneWeight.x, &vertex.boneIndex.x);
			vertex.position = { v.x,v.y,v.z };
            v = { vertex.normal.x, vertex.normal.y, vertex.normal.z, 0.0f };
			v = Skinning(v, &vertex.boneWeight.x, &vertex.boneIndex.x);
			vertex.normal = { v.x,v.y,v.z };
            v = { vertex.tangent.x, vertex.tangent.y, vertex.tangent.z, vertex.tangent.w };
            v = Skinning(v, &vertex.boneWeight.x, &vertex.boneIndex.x);
            vertex.tangent = v;

            vertex.boneWeight = { 1.0f,0.0f,0.0f,0.0f };
            vertex.boneIndex = { 0,0,0,0 };
        }
    }
    // ノードをルートノードのみにする
    _resource->GetAddressNodes().resize(1);
    _resource->GetAddressNodes()[0].parent = nullptr;
    _resource->GetAddressNodes()[0].children.clear();

    // ボーン情報削除
    for (ModelResource::Mesh& mesh : _resource->GetAddressMeshes())
    {
        mesh.nodeIndex = 0;
        mesh.bones.clear();
    }

    // ノードデータをコピー
    CopyNodes();

    /// COMオブジェクト生成
    CreateComObject(device, _filename.c_str());
}

// 不要ボーンの削除
void Model::RemoveUnusedNodes(ID3D11Device* device)
{
    // 必須ノードをマーク
    std::vector<bool> isNodeRequired(_resource->GetAddressNodes().size(), false);

    // メッシュ自体がアタッチされているノードをマーク
    for (const ModelResource::Mesh& mesh : _resource->GetAddressMeshes())
    {
        if (mesh.nodeIndex >= 0)
        {
            isNodeRequired[mesh.nodeIndex] = true;
        }
    }

    // 頂点ウェイトが0より大きいボーンインデックスを収集し、
    // そのボーンが参照するノードをマーク
    for (ModelResource::Mesh& mesh : _resource->GetAddressMeshes())
    {
        // このメッシュで実際に使用されている「ローカルボーンインデックス」のセット
        std::set<uint32_t> usedMeshBoneIndices;
        for (const ModelResource::Vertex& v : mesh.vertices)
        {
            if (v.boneWeight.x > 0.0f) usedMeshBoneIndices.insert(v.boneIndex.x);
            if (v.boneWeight.y > 0.0f) usedMeshBoneIndices.insert(v.boneIndex.y);
            if (v.boneWeight.z > 0.0f) usedMeshBoneIndices.insert(v.boneIndex.z);
            if (v.boneWeight.w > 0.0f) usedMeshBoneIndices.insert(v.boneIndex.w);
        }

        // 使用されているローカルボーンが参照する「グローバルノードインデックス」をマーク
        for (uint32_t localBoneIndex : usedMeshBoneIndices)
        {
            if (localBoneIndex < mesh.bones.size())
            {
                const ModelResource::Bone& bone = mesh.bones[localBoneIndex];
                if (bone.nodeIndex >= 0)
                {
                    isNodeRequired[bone.nodeIndex] = true;
                }
            }
        }
    }

    // 必須ノードの「親」をすべて必須としてマーク（階層を維持するため）
    for (size_t i = 0; i < _resource->GetAddressNodes().size(); ++i)
    {
        if (isNodeRequired[i])
        {
            int parentIdx = _resource->GetAddressNodes()[i].parentIndex;
            while (parentIdx != -1 && !isNodeRequired[parentIdx])
            {
                isNodeRequired[parentIdx] = true;
                parentIdx = _resource->GetAddressNodes()[parentIdx].parentIndex;
            }
        }
    }

    // ノードの再マッピングテーブルを作成し、_nodes を再構築
    std::vector<ModelResource::Node> newNodes;
    // oldToNewNodeIndex[古いインデックス] = 新しいインデックス (不要なノードは -1)
    std::vector<int> oldToNewNodeIndex(_resource->GetAddressNodes().size(), -1);
    int newIndex = 0;

    for (size_t i = 0; i < _resource->GetAddressNodes().size(); ++i)
    {
        if (isNodeRequired[i])
        {
            oldToNewNodeIndex[i] = newIndex;
            newNodes.push_back(_resource->GetAddressNodes()[i]); // この時点では古い parentIndex が入っている
            newIndex++;
        }
    }

    //ステップ 3: すべての参照インデックスを更新
    for (ModelResource::Node& node : newNodes)
    {
        if (node.parentIndex != -1)
        {
            node.parentIndex = oldToNewNodeIndex[node.parentIndex];
            _ASSERT_EXPR_A(node.parentIndex != -1, "Parent node was pruned, hierarchy is broken!");
        }
		// 子供情報は後で再構築するのでクリアしておく
		node.children.clear();
    }

    for (ModelResource::Mesh& mesh : _resource->GetAddressMeshes())
    {
        // メッシュのノードインデックスを更新
        mesh.nodeIndex = oldToNewNodeIndex[mesh.nodeIndex];
        _ASSERT_EXPR_A(mesh.nodeIndex != -1, "Mesh node was pruned!");

        // ボーンの再構築
        std::vector<ModelResource::Bone> newMeshBones;
        // oldToNewMeshBoneIndex[古いローカルボーンインデックス] = 新しいローカルボーンインデックス
        std::vector<uint32_t> oldToNewMeshBoneIndex(mesh.bones.size(), (uint32_t)-1);
        uint32_t newMeshBoneIdx = 0;

        for (size_t i = 0; i < mesh.bones.size(); ++i)
        {
            ModelResource::Bone& oldBone = mesh.bones[i];
            // このボーンが参照するノードが「必須ノード」として残っているか確認
            if (oldToNewNodeIndex[oldBone.nodeIndex] != -1)
            {
                // 必須ノードだったので、新しいボーンリストに追加
                oldBone.nodeIndex = oldToNewNodeIndex[oldBone.nodeIndex]; // グローバルノードインデックスを更新
                newMeshBones.push_back(oldBone);

                oldToNewMeshBoneIndex[i] = newMeshBoneIdx; // マッピングテーブルを更新
                newMeshBoneIdx++;
            }
        }
        // メッシュのボーンリストを入れ替え
        mesh.bones = std::move(newMeshBones);

        // 頂点の boneIndex を更新
        for (ModelResource::Vertex& v : mesh.vertices)
        {
            // ウェイトが 0 のインデックスは参照先が不定でも問題ないため、
            // 安全のために 0 (通常はルートボーン等) に設定する。
            v.boneIndex.x = (v.boneWeight.x > 0.0f) ? oldToNewMeshBoneIndex[v.boneIndex.x] : 0;
            v.boneIndex.y = (v.boneWeight.y > 0.0f) ? oldToNewMeshBoneIndex[v.boneIndex.y] : 0;
            v.boneIndex.z = (v.boneWeight.z > 0.0f) ? oldToNewMeshBoneIndex[v.boneIndex.z] : 0;
            v.boneIndex.w = (v.boneWeight.w > 0.0f) ? oldToNewMeshBoneIndex[v.boneIndex.w] : 0;

            // ウェイトが 0 より大きいのに、マッピング先が -1 (pruned) だったらアサーション
            // (ステップ 1-2 のロジックが正しければ、これは発生しない)
            if (v.boneWeight.x > 0.0f) _ASSERT_EXPR_A(v.boneIndex.x != (uint32_t)-1, "Vertex weighted to a pruned bone!");
            if (v.boneWeight.y > 0.0f) _ASSERT_EXPR_A(v.boneIndex.y != (uint32_t)-1, "Vertex weighted to a pruned bone!");
            if (v.boneWeight.z > 0.0f) _ASSERT_EXPR_A(v.boneIndex.z != (uint32_t)-1, "Vertex weighted to a pruned bone!");
            if (v.boneWeight.w > 0.0f) _ASSERT_EXPR_A(v.boneIndex.w != (uint32_t)-1, "Vertex weighted to a pruned bone!");
        }
    }

    // ノードリストを入れ替え
    _resource->GetAddressNodes().clear();
	_resource->GetAddressNodes() = std::move(newNodes);

    // ノードポインタ（parent, children）を再構築
    _resource->BuildNode(_resource->GetAddressNodes());

    // メッシュ内のボーンポインタ（bone.node）を再構築
    _resource->BuildBone(_resource->GetAddressMeshes(), _resource->GetAddressNodes());

    // ノードデータをコピー
    CopyNodes();

    // COMオブジェクト生成
    CreateComObject(device, _filename.c_str());
}

void Model::ReSerialize()
{
    _resource->Serialize(_filename.c_str());
}

/// ノードデータのコピー
void Model::CopyNodes()
{
    _poseNodes.clear();
    _poseNodes.resize(_resource->GetNodes().size());
    for (size_t i = 0; i < _poseNodes.size(); ++i)
    {
        const ModelResource::Node& node = _resource->GetNodes().at(i);
        ModelResource::Node& copyNode = _poseNodes.at(i);
        // データをコピー
        copyNode.name = node.name;
        copyNode.parentIndex = node.parentIndex;
        copyNode.position = node.position;
        copyNode.rotation = node.rotation;
        copyNode.scale = node.scale;
    }

    // 親、子供の再構築
    for (size_t i = 0; i < _poseNodes.size(); ++i)
    {
        ModelResource::Node& copyNode = _poseNodes.at(i);

        if (copyNode.parentIndex != -1)
        {
            copyNode.parent = &_poseNodes.at(copyNode.parentIndex);
            _poseNodes.at(copyNode.parentIndex).children.emplace_back(&copyNode);
        }
    }
}

/// COMオブジェクト生成
void Model::CreateComObject(ID3D11Device* device, const char* fbxFilename)
{
    for (ModelResource::Mesh& mesh : _resource->GetAddressMeshes())
    {
        HRESULT hr{ S_OK };
        D3D11_BUFFER_DESC buffer_desc{};
        D3D11_SUBRESOURCE_DATA subresource_data{};
        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(ModelResource::Vertex) * mesh.vertices.size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        buffer_desc.CPUAccessFlags = 0;
        buffer_desc.MiscFlags = 0;
        buffer_desc.StructureByteStride = 0;
        subresource_data.pSysMem = mesh.vertices.data();
        subresource_data.SysMemPitch = 0;
        subresource_data.SysMemSlicePitch = 0;
        hr = device->CreateBuffer(&buffer_desc, &subresource_data,
            mesh.vertexBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        subresource_data.pSysMem = mesh.indices.data();
        hr = device->CreateBuffer(&buffer_desc, &subresource_data,
            mesh.indexBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }
    for (ModelResource::Material& modelMaterial : _resource->GetAddressMaterials())
    {
        auto& material = _materialMap.emplace_back();
        material.SetName(modelMaterial.name);
        // テクスチャ情報の取得
        for (auto& [key, textureData] : modelMaterial.textureDatas)
        {
            if (textureData.filename.size() > 0)
            {
                std::filesystem::path path(fbxFilename);
                // textureData.filenameの先頭が"Texture"なら相対パス化
                if (textureData.filename.find("Texture") == 0)
                {
                    path.replace_filename(textureData.filename);
                    material.LoadTexture(key, path.c_str());
                }
                else
                {
                    // それ以外は絶対パスなのでそのまま読み込む
                    path = textureData.filename;
                    material.LoadTexture(key, path.c_str());
                }
            }
            else
            {
                material.MakeDummyTexture(key,
                    textureData.dummyTextureValue,
                    textureData.dummyTextureDimension);
            }
        }

        // カラー情報の取得
        for (auto& [key, color] : modelMaterial.colors)
        {
            material.SetColor(key, color);
        }

        // シェーダーの初期設定
        material.SetShaderName("PBR");
    }
}
