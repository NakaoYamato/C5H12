#include "SceneModelEditor.h"

#include <filesystem>
#include <imgui.h>

#include "../../Library/Graphics/Graphics.h"

#include "../../DebugSupporter/DebugSupporter.h"

void SceneModelEditor::Initialize()
{
    Scene::Initialize();

    ID3D11Device* device = Graphics::Instance().GetDevice();

    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

    // 編集用オブジェクト作成
    ActorManager& actorManager = GetActorManager();
	_modelActor = RegisterActor<Actor>(u8"Model", ActorTag::Stage);
    _modelActor.lock()->GetTransform().SetLengthScale(0.01f);
    _modelRenderer = _modelActor.lock()->AddComponent<ModelRenderer>("Data/Model/Shape/Box.fbx");
    _animator = _modelActor.lock()->AddComponent<Animator>(_modelRenderer.lock()->GetModel());

    // デバッグカメラをオンにする
    Debug::GetDebugInput()->buttonData |= DebugInput::BTN_F4;
}

void SceneModelEditor::Update(float elapsedTime)
{
	Scene::Update(elapsedTime);

    // グリッド表示
    Debug::Renderer::DrawGrid(10);
    Debug::Renderer::DrawAxis(DirectX::XMFLOAT4X4(
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
        ));

    auto model = _modelRenderer.lock()->GetModel();
    if (model && _animator.lock()->GetCurrentAnimIndex() != -1)
    {
        // ボーン表示
        DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        // 親から子どもにボーンをのばす
        for (auto& node : model->GetPoseNodes())
        {
            if (node.parent != nullptr)
            {
                Vector3 childWP = Vector3(node.worldTransform._41, node.worldTransform._42, node.worldTransform._43);
                Vector3 parentWP = Vector3(node.parent->worldTransform._41, node.parent->worldTransform._42, node.parent->worldTransform._43);
                float length = Vec3Length(parentWP - childWP);
                if (length == 0.0f)
                    continue;
                DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&parentWP), DirectX::XMLoadFloat3(&childWP), Up);
                DirectX::XMFLOAT4X4 world;
                DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixInverse(nullptr, View));
                Debug::Renderer::DrawBone(
                    world,
                    length,
                    _boneColor
                );
            }
        }

        float animCurrentTime = _animator.lock()->GetCurrentAnimSeconds();
        // 当たり判定表示
        for (auto& keyframe : _animCollisionData.GetKeyframes(_animator.lock()->GetCurrentAnimationName()))
        {
            if (animCurrentTime > keyframe.startSeconds &&
                animCurrentTime < keyframe.endSeconds)
            {
                auto& node = model->GetPoseNodes()[keyframe.nodeIndex];
                DirectX::XMMATRIX T = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&keyframe.position));
                DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(keyframe.angle.x, keyframe.angle.y, keyframe.angle.z);
                DirectX::XMMATRIX S = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&keyframe.scale));
                DirectX::XMFLOAT4X4 transform = {};

                DirectX::XMStoreFloat4x4(&transform, S * R * T * DirectX::XMLoadFloat4x4(&node.worldTransform));

                switch (keyframe.shapeType)
                {
                case AnimationCollisionData::ShapeType::Box:
                    Debug::Renderer::DrawBox(transform, _VECTOR4_WHITE);
                    break;
                case AnimationCollisionData::ShapeType::Sphere:
                    Debug::Renderer::DrawSphere(Vector3(transform._41, transform._42, transform._43), keyframe.scale.x, _VECTOR4_WHITE);
                    break;
                case AnimationCollisionData::ShapeType::Capsule:
                    Debug::Renderer::DrawCapsule(transform, 1.0f, 1.0f, _VECTOR4_WHITE);
                    break;
                }
            }
        }
    }
}

void SceneModelEditor::DrawGui()
{
    if (ImGui::Begin(u8"モデル"))
    {
        _modelActor.lock()->DrawGui();
    }
    ImGui::End();

    if (ImGui::Begin(u8"編集"))
    {
        ImGui::Text(u8"絶対パス：%s", _filepath.c_str());
        ImGui::Text(u8"カレントディレクトリ：%s", _currentDirectory.c_str());
        ImGui::Text(u8"相対パス：%s", _relativePath.c_str());
        ImGui::Separator();
        ImGui::Text(u8"アニメーション判定");
        if (_animator.lock()->GetCurrentAnimIndex() != -1)
        {
            if (ImGui::TreeNode(u8"現在のアニメーション判定"))
            {
                _animCollisionData.DrawGui(_animator.lock()->GetCurrentAnimationName(), _nodeNames);

                ImGui::TreePop();
            }
        }
        if (ImGui::TreeNode(u8"すべてのアニメーション判定"))
        {
            _animCollisionData.DrawGuiAll(_nodeNames);

            ImGui::TreePop();
        }
        if (ImGui::Button(u8"判定の書き出し"))
        {
            std::filesystem::path serializePath(_filepath);
            serializePath.replace_extension(".animCol");
            _animCollisionData.Serialize(serializePath.string().c_str());
        }

        ImGui::Separator();

        ImGui::ColorEdit4(u8"ボーンの色", &_boneColor.x);
    }
    ImGui::End();

    // ファイル操作
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(u8"ファイル"))
        {
            if (ImGui::Button(u8"モデルを開く"))
            {
                std::string filepath;
                std::string currentDirectory;
                Debug::Dialog::OpenFileName(filepath, currentDirectory);
                _filepath = filepath;
                _currentDirectory = currentDirectory;
                try
                {
                    std::filesystem::path path =
                        std::filesystem::relative(filepath, currentDirectory);
                    _relativePath = path.u8string();

                    Model test(Graphics::Instance().GetDevice(), _relativePath.c_str());
                }
                catch (...)
                {
                    _relativePath = "error";
                }

                _modelRenderer.lock()->LoadModel(_filepath.c_str());
                _animator.lock()->SetModel(_modelRenderer.lock()->GetModel());

                _nodeNames.clear();
                // ノードの名前を全取得
                for (auto& node : _modelRenderer.lock()->GetModel()->GetPoseNodes())
                {
                    _nodeNames.push_back(node.name.c_str());
                }

                // 判定のシリアライズの確認
                std::filesystem::path serializePath(_filepath);
                serializePath.replace_extension(".animCol");
                if (std::filesystem::exists(serializePath))
                {
                    // デシリアライズ
                    _animCollisionData.Deserialize(serializePath.string().c_str());
                }
                else
                {
                    // 新規作成
                    _animCollisionData.Clear();
                    // アニメーション名を登録
                    for (auto& animation : _modelRenderer.lock()->GetModel()->GetResource()->GetAnimations())
                    {
                        _animCollisionData.AddKeyFrames(animation.name);
                    }
                }
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}
