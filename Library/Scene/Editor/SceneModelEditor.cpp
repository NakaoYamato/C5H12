#include "SceneModelEditor.h"

#include <algorithm>
#include <filesystem>
#include <imgui.h>

#include "../../Library/Math/Quaternion.h"
#include "../../Library/Graphics/Graphics.h"

#include "../../DebugSupporter/DebugSupporter.h"

#include "../../Library/Exporter/Exporter.h"
#include "../../Library/PostProcess/PostProcessManager.h"

void SceneModelEditor::OnInitialize()
{
    ID3D11Device* device = Graphics::Instance().GetDevice();

    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

    // 編集用オブジェクト作成
    ActorManager& actorManager = GetActorManager();
	_modelActor = RegisterActor<Actor>(u8"Model", ActorTag::Stage);
    _modelActor.lock()->LoadModel("Data/Model/Shape/Box.fbx");
    _modelActor.lock()->GetTransform().SetLengthScale(0.01f);
    _modelRenderer = _modelActor.lock()->AddComponent<ModelRenderer>();
    _animator = _modelActor.lock()->AddComponent<Animator>();

    // デバッグカメラをオンにする
    Debug::GetDebugInput()->buttonData |= DebugInput::BTN_F4;

    // グリッド表示
	SetShowGrid(true);
}

void SceneModelEditor::OnUpdate(float elapsedTime)
{
    Debug::Renderer::DrawAxis(DirectX::XMFLOAT4X4(
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
        ));

    auto model = _modelActor.lock()->GetModel().lock();
    if (model)
    {
        // ルートモーション移動値を足す
		if (_animator.lock()->GetAnimationIndex() != -1)
		{
			Vector3 rootMotion = _animator.lock()->GetRootMovement();
			_modelActor.lock()->GetTransform().SetPosition(_modelActor.lock()->GetTransform().GetPosition() + rootMotion);
		}

        // 当たり判定表示
        if (_showCollision)
            _modelCollision.DebugRender();

        // ノード表示
        if (_showNode)
            model->DebugDrawNode(_nodeColor);

        // 頂点表示
        if (_selectingMeshIndex != -1)
        {
            auto& meshes = model->GetResource()->GetAddressMeshes();
            auto& mesh = meshes[_selectingMeshIndex];


            Debug::Renderer::AddVertex(Vector3::Zero);
            for (auto& vertex : mesh.vertices)
            {
                Vector3 position = Vector3::Zero;
                size_t indices[4] = { vertex.boneIndex.x, vertex.boneIndex.y, vertex.boneIndex.z, vertex.boneIndex.w };
                float weight[4] = { vertex.boneWeight.x, vertex.boneWeight.y, vertex.boneWeight.z, vertex.boneWeight.w };
                if (mesh.bones.size() != 0)
                {
                    for (size_t i = 0; i < 4; ++i)
                    {
                        const ModelResource::Bone& bone = mesh.bones.at(indices[i]);
                        DirectX::XMMATRIX World = DirectX::XMLoadFloat4x4(&model->GetPoseNodes()[bone.nodeIndex].worldTransform);
                        DirectX::XMMATRIX Offset = DirectX::XMLoadFloat4x4(&bone.offsetTransform);
                        DirectX::XMMATRIX Bone = Offset * World;
                        position += Vector3::TransformCoord(vertex.position, Bone) * weight[i];
                    }
                }
                else
                {
                    position = Vector3::TransformCoord(vertex.position, _modelActor.lock()->GetTransform().GetMatrix());
                }
				Debug::Renderer::AddVertex(position);
				Debug::Renderer::AddVertex(position);
                //Debug::Renderer::DrawSphere(position, _vertexPointRadius, _vertexPointColor);
            }
            Debug::Renderer::AddVertex(Vector3::Zero);
        }
        
        // アニメーションを再生中は当たり判定を表示
        if (_animator.lock()->GetAnimationIndex() != -1)
        {
            // 当たり判定表示
			_animationEvent.DebugRender(_animator.lock()->GetAnimationName(), _animator.lock()->GetAnimationTimer());
        }
    }


    if (::GetAsyncKeyState('C') & 0x8000)
    {
        // 画面キャプチャ
		std::wstring filename = L"./Data/Debug/Capture/" + std::to_wstring(GetTickCount64()) + L".png";

		if (Exporter::SavePngFile(
            Graphics::Instance().GetDevice(),
            Graphics::Instance().GetDeviceContext(),
            PostProcessManager::Instance().GetAppliedEffectSRV().Get(),
            filename))
		{
		}
    }
}

void SceneModelEditor::DrawGui()
{
    // モデルのGUI描画
    DrawModelGui();
    // アニメーションイベントGUI描画
    DrawAnimationEventGui();
    // モデル当たり判定GUI描画
    DrawModelColliderGui();
    // アニメーション編集GUI描画
    DrawEditAnimationGui();
    // テクスチャのGUI描画
    DrawTextureGui();
    // メニューバーのGUI描画
    DrawMenuBarGui();
	// アニメーション追加GUI描画
    DrawAddAnimationGui();
}

// メニューバーのGUI描画
void SceneModelEditor::DrawMenuBarGui()
{
    // ファイル操作
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(u8"ファイル"))
        {
            if (ImGui::Button(u8"モデルを開く"))
            {
                // ダイアログを開く
                std::string filepath;
                std::string currentDirectory;
                Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory);
                // ファイルを選択したら
                if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
                {
                    // 絶対パス、カレントディレクトリ取得
                    _filepath = filepath;
                    _currentDirectory = currentDirectory;
                    try
                    {
                        // 相対パス取得
                        std::filesystem::path path =
                            std::filesystem::relative(filepath, currentDirectory);
                        _relativePath = path.u8string();
                    }
                    catch (...)
                    {
                        _relativePath.clear();
                    }

                    // マテリアル削除
                    _modelRenderer.lock()->GetMaterials().clear();

                    // モデルの読み込み
                    auto model = _modelActor.lock()->LoadModel(_relativePath.size() != 0 ? _relativePath.c_str() : _filepath.c_str());
                    _animator.lock()->ResetModel(model.lock());

					// アニメーションイベントの読み込み
                    _animationEvent.Load(model.lock());
					// モデルの衝突判定の読み込み
					_modelCollision.Load(model.lock());
                }
            }

            if (ImGui::Button(u8"アニメーション読み込み"))
            {
                // ダイアログを開く
                std::string filepath;
                std::string currentDirectory;
                Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory);
                // ファイルを選択したら
                if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
                {
					_animationModel = std::make_shared<Model>(Graphics::Instance().GetDevice(), filepath.c_str());
                    _addAnimationMap.resize(_animationModel->GetResource()->GetAddressAnimations().size());
                }
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

// モデルGUI描画
void SceneModelEditor::DrawModelGui()
{
    if (ImGui::Begin(u8"Transform"))
    {
        if (_modelActor.lock())
            _modelActor.lock()->GetTransform().DrawGui();
    }
    ImGui::End();
    if (ImGui::Begin(u8"ModelRenderer"))
    {
        if (_modelRenderer.lock())
            _modelRenderer.lock()->DrawGui();
    }
    ImGui::End();
    if (ImGui::Begin(u8"Animator"))
    {
        if (_animator.lock())
            _animator.lock()->DrawGui();
    }
    ImGui::End();
    if (ImGui::Begin(u8"ModelDebug"))
    {
        ImGui::Text(u8"絶対パス：%s", _filepath.c_str());
        ImGui::Text(u8"カレントディレクトリ：%s", _currentDirectory.c_str());
        ImGui::Text(u8"相対パス：%s", _relativePath.c_str());
        ImGui::Separator();
        ImGui::Checkbox(u8"ノード表示", &_showNode);
        ImGui::Checkbox(u8"当たり判定表示", &_showCollision);
        if (ImGui::Button(u8"モデル表示"))
        {
            _modelActor.lock()->SetShowFlag(!_modelActor.lock()->IsShowing());
        }
        ImGui::SliderInt(u8"選択中のメッシュ", &_selectingMeshIndex, -1, (int)_modelActor.lock()->GetModel().lock()->GetResource()->GetAddressMeshes().size() - 1);
        ImGui::DragFloat(u8"頂点の点の大きさ", &_vertexPointRadius, 0.01f);
        ImGui::ColorEdit4(u8"頂点の色", &_vertexPointColor.x);

        ImGui::ColorEdit4(u8"ノードの色", &_nodeColor.x);
    }
    ImGui::End();
}

// アニメーションイベントGUI描画
void SceneModelEditor::DrawAnimationEventGui()
{
    if (ImGui::Begin(u8"アニメーションイベント"))
    {
        if (ImGui::TreeNode(u8"メッセージリスト"))
        {
            // メッセージリストの編集
            _animationEvent.DrawMassageListGui();
            ImGui::TreePop();
        }
        ImGui::Separator();
        ImGui::Text(u8"アニメーション判定");
        if (_animator.lock()->GetAnimationIndex() != -1)
        {
            if (ImGui::TreeNode(u8"現在のアニメーション判定"))
            {
                _animationEvent.DrawGui(_animator.lock()->GetAnimationName());

                ImGui::TreePop();
            }
        }
        if (ImGui::TreeNode(u8"すべてのアニメーション判定"))
        {
            _animationEvent.DrawGui();

            ImGui::TreePop();
        }
        if (ImGui::Button(u8"判定の書き出し"))
        {
            _animationEvent.Serialize(_filepath.c_str());
        }
    }
    ImGui::End();
}

// モデル当たり判定GUI描画
void SceneModelEditor::DrawModelColliderGui()
{
    if (ImGui::Begin(u8"当たり判定"))
    {
        _modelCollision.DrawGui(true);
        if (ImGui::Button(u8"当たり判定の書き出し"))
        {
            _modelCollision.Serialize(_filepath.c_str());
        }
    }
    ImGui::End();
}

// アニメーション編集GUI描画
void SceneModelEditor::DrawEditAnimationGui()
{
    if (ImGui::Begin(u8"モデルデータ編集"))
    {
        auto model = _modelActor.lock()->GetModel().lock();

        if (ImGui::TreeNode(u8"ノード"))
        {
            std::function<void(ModelResource::Node&)> NodeGui = [&](ModelResource::Node& node)
                {
                    if (ImGui::TreeNodeEx(&node, ImGuiTreeNodeFlags_DefaultOpen, node.name.c_str()))
                    {
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
			NodeGui(model->GetResource()->GetAddressNodes()[0]);
            ImGui::TreePop();
        }

        if (ImGui::Button(u8"すべてのアニメーションを90度回転"))
        {
            DirectX::XMVECTOR RotationY90 = DirectX::XMQuaternionRotationAxis(
                DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
                DirectX::XMConvertToRadians(90.0f));
            for (auto& animation : model->GetResource()->GetAddressAnimations())
            {
                auto& rootAnimNode = animation.nodeAnims[0];
                for (auto& frame : rootAnimNode.rotationKeyframes)
                {
                    DirectX::XMVECTOR Q = DirectX::XMLoadFloat4(&frame.value);

                    DirectX::XMStoreFloat4(&frame.value, DirectX::XMQuaternionMultiply(RotationY90, Q));
                }
            }
        }
        ImGui::Separator();
        ImGui::InputText(u8"取り除く対象", &_filterAnimationName);
        if (ImGui::Button(u8"アニメーション名を取り除く"))
        {
            auto& animations = model->GetResource()->GetAddressAnimations();
            for (size_t i = 0; i < animations.size(); ++i)
            {
                size_t pos = animations[i].name.find(_filterAnimationName);
                if (pos != std::string::npos)
                {
                    animations[i].name.erase(pos, _filterAnimationName.length());
                }
            }
        }
        ImGui::Separator();
        if (ImGui::Button(u8"アニメーションのソート"))
        {
            std::sort(model->GetResource()->GetAddressAnimations().begin(),
                model->GetResource()->GetAddressAnimations().end(),
                [](const ModelResource::Animation& a, const ModelResource::Animation& b)
                {
                    return a.name < b.name;
                });
        }
        ImGui::Separator();

        // アニメーション再生中か確認
        int currentAnimIndex = _animator.lock()->GetAnimationIndex();
        if (currentAnimIndex == -1)
        {
            ImGui::End();
            return;
        }
        ModelResource::Animation& animation = model->GetResource()->GetAddressAnimations()[currentAnimIndex];

        int index = 0;
        for (auto& node : animation.nodeAnims)
        {
            if (ImGui::TreeNode(model->GetPoseNodes()[index].name.c_str()))
            {
                if (ImGui::TreeNode(u8"Position"))
                {
                    int keyIndex = 0;
                    for (auto& frame : node.positionKeyframes)
                    {
                        if (ImGui::TreeNode(std::to_string(keyIndex).c_str()))
                        {
                            ImGui::DragFloat3(u8"position", &frame.value.x, 0.1f);
                            ImGui::TreePop();
                        }
                        keyIndex++;
                    }
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode(u8"Rotation"))
                {
                    int keyIndex = 0;
                    for (auto& frame : node.rotationKeyframes)
                    {
                        if (ImGui::TreeNode(std::to_string(keyIndex).c_str()))
                        {
                            {
                                DirectX::XMFLOAT4X4 transform = Quaternion(frame.value).ToMatrix();
                                if (Debug::Guizmo(GetMainCamera()->GetView(), GetMainCamera()->GetProjection(),
                                    &transform))
                                {
                                    // 回転を取得
                                    DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&transform);
                                    DirectX::XMVECTOR S, R, T;
                                    DirectX::XMMatrixDecompose(&S, &R, &T, M);
                                    DirectX::XMStoreFloat4(&frame.value, R);
                                }
                            }
                            ImGui::DragFloat4(u8"oriental", &frame.value.x, 0.1f);
                            Vector3 degrees = Vector3::ToDegrees(Quaternion::ToRollPitchYaw(frame.value));
                            if (ImGui::DragFloat3(u8"angle", &degrees.x, 0.1f))
                            {
                                frame.value = Quaternion::FromRollPitchYaw(Vector3::ToRadians(degrees));
                            }
                            if (ImGui::Button("EDIT"))
                            {
                                frame.value.x = 0.0f;
                                frame.value.y = -0.707f;
                                frame.value.z = 0.707f;
                                frame.value.w = 0.0f;
                            }
                            ImGui::TreePop();
                        }
                        keyIndex++;
                    }
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode(u8"Scale"))
                {
                    int keyIndex = 0;
                    for (auto& frame : node.scaleKeyframes)
                    {
                        if (ImGui::TreeNode(std::to_string(keyIndex).c_str()))
                        {
                            ImGui::DragFloat3(u8"scale", &frame.value.x, 0.1f);
                            ImGui::TreePop();
                        }
                        keyIndex++;
                    }
                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }
            index++;
        }
    }
    ImGui::End();
}

// アニメーション追加GUI描画
void SceneModelEditor::DrawAddAnimationGui()
{
	if (_animationModel.get() == nullptr)
		return;

	if (ImGui::Begin(u8"アニメーション追加"))
	{
        auto& animations = _animationModel->GetResource()->GetAddressAnimations();
        int index = 0;
        for (const ModelResource::Animation& animation : animations)
        {
            ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

            bool flag = _addAnimationMap[index];
			ImGui::Checkbox(animation.name.c_str(), &flag);
			_addAnimationMap[index] = flag;

            index++;
        }
        ImGui::Separator();
		if (ImGui::Button(u8"アニメーション追加"))
		{
			auto model = _modelActor.lock()->GetModel().lock();
			auto& animations = _animationModel->GetResource()->GetAddressAnimations();
			for (size_t i = 0; i < animations.size(); ++i)
			{
				if (_addAnimationMap[i])
				{
					model->GetResource()->AppendAnimations(
                        animations[i],
                        _animationModel->GetResource()->GetNodes());
				}
			}

            _animationModel.reset();
            _addAnimationMap.clear();
		}
	}
	ImGui::End();
}

// テクスチャのGUI描画
void SceneModelEditor::DrawTextureGui()
{
    if (ImGui::Begin(u8"テクスチャ編集"))
    {
        auto model = _modelActor.lock()->GetModel().lock();
        auto& modelMaterials = model->GetResource()->GetAddressMaterials();
        auto& materials = _modelRenderer.lock()->GetMaterials();
        int index = 0;
        for (auto& modelMaterial : modelMaterials)
        {
            if (ImGui::TreeNode(modelMaterial.name.c_str()))
            {
                for (auto& [key, textureData] : modelMaterial.textureDatas)
                {
                    ImGui::Text("%s", key.c_str());
                    ImGui::SameLine();
                    ImGui::Text("%s", textureData.filename.c_str());
                    static float textureSize = 128.0f;
                    ImGui::Image(materials[index].GetTextureSRV(key), { textureSize,textureSize });
                    ImGui::PushID(&textureData.filename);
                    ImGui::SameLine();
                    if (ImGui::Button("..."))
                    {
                        // ダイアログを開く
                        std::string filepath;
                        std::string currentDirectory;
                        const char* filter = "Texture Files(*.dds;*.png;*.tga;*.jpg;*.tif)\0*.dds;*.png;*.tga;*.jpg;*.tif;\0All Files(*.*)\0*.*;\0\0";
                        Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, filter);
                        // ファイルを選択したら
                        if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
                        {
                            // 相対パス取得
                            std::filesystem::path path =
                                std::filesystem::relative(filepath, currentDirectory);
                            textureData.filename = path.u8string();
                            materials[index].LoadTexture(key, path.c_str());
                        }
                    }
                    if (ImGui::Button("削除"))
                    {
                        // テクスチャを削除
                        textureData.filename = "";
                        materials[index].MakeDummyTexture(key, 0xFF0000FF, 1);
                    }
                    ImGui::PopID();
                }
                ImGui::TreePop();
            }

            index++;
        }
    }
    ImGui::End();
}
