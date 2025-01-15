#include "Model.h"

#include <sstream>
#include <functional>
#include <filesystem>
#include <imgui.h>

#include "../HRTrace.h"
#include "../ResourceManager/ModelResourceManager.h"
#include "../ResourceManager/GpuResourceManager.h"
#include "../Math/Quaternion.h"

#include "../DebugSupporter/DebugSupporter.h"

Model::Model(ID3D11Device* device, const char* filename) : 
    filepath(filename)
{
    // リソース読み込み
    resource = ModelResourceManager::Instance().LoadModelResource(filename);

    assert(resource != nullptr);

    // ノードデータをコピー
    copyNodes.resize(resource->GetNodes().size());
    for (size_t i = 0; i < copyNodes.size(); ++i)
    {
        const ModelResource::Node& node = resource->GetNodes().at(i);
        ModelResource::Node& copyNode = copyNodes.at(i);
        // データをコピー
        copyNode.name = node.name;
        copyNode.parentIndex = node.parentIndex;
        copyNode.position = node.position;
        copyNode.rotation = node.rotation;
        copyNode.scale = node.scale;
    }

    // 親、子供の再構築
    for (size_t i = 0; i < copyNodes.size(); ++i)
    {
        ModelResource::Node& copyNode = copyNodes.at(i);

        if (copyNode.parentIndex != -1)
        {
            copyNode.parent = &copyNodes.at(copyNode.parentIndex);
            copyNodes.at(copyNode.parentIndex).children.emplace_back(&copyNode);
        }
    }

    DebugSupporter::Instance().OutputString(L"FBXモデルの読み込み成功\n");
    DebugSupporter::Instance().OutputString("\t");
    DebugSupporter::Instance().OutputString(filename);
    DebugSupporter::Instance().OutputString("\n");

    // COM生成
    CreateComObject(device, filename);
}

// トランスフォーム更新処理
void Model::UpdateTransform(const DirectX::XMFLOAT4X4& world)
{
    for (ModelResource::Node& node : copyNodes)
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

void Model::DrawGui()
{
    if (ImGui::TreeNode(u8"モデル"))
    {
        if (ImGui::TreeNode(u8"マテリアル"))
        {
            for (auto& material : resource->GetAddressMaterials())
            {
                if (ImGui::TreeNodeEx(&material, ImGuiTreeNodeFlags_Leaf, material.name.c_str()))
                {
                    for (auto& [key, color] : material.colors)
                    {
                        ImGui::ColorEdit4(key.c_str(), &color.x);
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode(u8"ノード"))
        {
            std::function<void(ModelResource::Node&)> NodeGui = [&](ModelResource::Node& node)
                {
                    if (ImGui::TreeNodeEx(&node, ImGuiTreeNodeFlags_DefaultOpen, node.name.c_str()))
                    {
                        // ダブルクリックで選択
                        if (ImGui::IsItemClicked())
                        {
                            debugNodeIndex = GetNodeIndex(node.name);
                        }
                        ImGui::Separator();
                        ImGui::DragFloat3(u8"position", &node.position.x, 0.1f);
                        Vector3 degrees = Vec3ConvertToDegrees(QuaternionToRollPitchYaw(node.rotation));
                        if (ImGui::DragFloat3(u8"degrees", &degrees.x, 0.1f))
                        {
                            node.rotation = QuaternionFromRollPitchYaw(Vec3ConvertToRadians(degrees));
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
            NodeGui(copyNodes.at(0));
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    if (debugNodeIndex != -1)
    {
        Debug::DrawAxis(copyNodes[debugNodeIndex].worldTransform);        
    }
}

void Model::CreateComObject(ID3D11Device* device, const char* fbx_filename)
{
    for (ModelResource::Mesh& mesh : resource->GetAddressMeshes())
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

    HRESULT hr{ S_OK };

    // シェーダーリソースビューの作成
    for (ModelResource::Material& material : resource->GetAddressMaterials())
    {
        if (material.textureDatas.at("Diffuse").filename.size() > 0)
        {
            std::filesystem::path path(fbx_filename);
            path.replace_filename(material.textureDatas.at("Diffuse").filename);
            D3D11_TEXTURE2D_DESC texture2d_desc;
            GpuResourceManager::LoadTextureFromFile(device, path.c_str(),
                material.textureDatas.at("Diffuse").textureSRV.ReleaseAndGetAddressOf(),
                &texture2d_desc);
        }
        else
        {
            GpuResourceManager::MakeDummyTexture(device,
                material.textureDatas.at("Diffuse").textureSRV.ReleaseAndGetAddressOf(),
                0xFFFFFFFF,
                16);
        }
        if (material.textureDatas.at("Normal").filename.size() > 0)
        {
            std::filesystem::path path(fbx_filename);
            path.replace_filename(material.textureDatas.at("Normal").filename);
            D3D11_TEXTURE2D_DESC texture2d_desc;
            GpuResourceManager::LoadTextureFromFile(device, path.c_str(),
                material.textureDatas.at("Normal").textureSRV.ReleaseAndGetAddressOf(),
                &texture2d_desc);
        }
        else
        {
            GpuResourceManager::MakeDummyTexture(device,
                material.textureDatas.at("Normal").textureSRV.ReleaseAndGetAddressOf(),
                0xFFFF7F7F, // Normal == 0xFFFF7F7F
                16);
        }
        if (material.textureDatas.at("Specular").filename.size() > 0)
        {
            std::filesystem::path path(fbx_filename);
            path.replace_filename(material.textureDatas.at("Specular").filename);
            D3D11_TEXTURE2D_DESC texture2d_desc;
            GpuResourceManager::LoadTextureFromFile(device, path.c_str(),
                material.textureDatas.at("Specular").textureSRV.ReleaseAndGetAddressOf(),
                &texture2d_desc);
        }
        else
        {
            GpuResourceManager::MakeDummyTexture(device,
                material.textureDatas.at("Specular").textureSRV.ReleaseAndGetAddressOf(),
                0xFFFFFF00,// ダミーならアルファが0 
                16);
        }
    }
}
