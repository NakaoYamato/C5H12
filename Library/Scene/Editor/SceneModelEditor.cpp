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
}

void SceneModelEditor::DrawGui()
{
    if (ImGui::Begin(u8"モデル"))
    {
        ImGui::Text(u8"%s", _filepath.c_str());
        ImGui::Text(u8"%s", _currentDirectory.c_str());
        ImGui::Text(u8"%s", _relativePath.c_str());

        _modelActor.lock()->DrawGui();
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
