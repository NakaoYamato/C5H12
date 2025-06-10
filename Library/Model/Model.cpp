#include "Model.h"

#include <sstream>
#include <functional>
#include <filesystem>
#include <imgui.h>

#include "../HRTrace.h"
#include "ModelResourceManager.h"
#include "../Graphics/GpuResourceManager.h"
#include "../Math/Quaternion.h"

#include "../DebugSupporter/DebugSupporter.h"

Model::Model(ID3D11Device* device, const char* filename)
{
    // リソース読み込み
    _resource = ModelResourceManager::Instance().LoadModelResource(filename);

    assert(_resource != nullptr);

    // シリアライズ用ファイルパス取得
    _filename = _resource->GetSerializePath();

    // ノードデータをコピー
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

    Debug::Output::String(L"FBXモデルの読み込み成功\n");
    Debug::Output::String("\t");
    Debug::Output::String(filename);
    Debug::Output::String("\n");

    /// COMオブジェクト生成
    CreateComObject(device, filename);
}

// トランスフォーム更新処理
void Model::UpdateTransform(const DirectX::XMFLOAT4X4& world)
{
    for (ModelResource::Node& node : _poseNodes)
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

    if (_debugNodeIndex != -1)
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

void Model::ReSerialize()
{
    _resource->Serialize(_filename.c_str());
}

/// COMオブジェクト生成
void Model::CreateComObject(ID3D11Device* device, const char* fbx_filename)
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
}
