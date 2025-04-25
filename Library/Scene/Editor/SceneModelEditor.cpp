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

    // ボーン表示
    auto model = _modelRenderer.lock()->GetModel();
    if (model)
    {
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

        ImGui::ColorEdit4(u8"ボーンの色", &_boneColor.x);
    }
    ImGui::End();

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
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}
