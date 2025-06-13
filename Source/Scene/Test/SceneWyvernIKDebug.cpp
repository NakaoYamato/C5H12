#include "SceneWyvernIKDebug.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../DebugSupporter/DebugSupporter.h"
#include "../../Library/Math/MathF.h"

#include <imgui.h>
#include <ImGuizmo.h>

void SceneWyvernIKDebug::Initialize()
{
    Scene::Initialize();

    ID3D11Device* device = Graphics::Instance().GetDevice();

    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

	_playerIK = false;
    auto modelActor = RegisterActor<Actor>(u8"Model", ActorTag::Stage);
    modelActor->LoadModel("./Data/Model/Dragons/Kuzar the Magnificent.fbx");
    _model = modelActor->GetModel();
    modelActor->GetTransform().SetLengthScale(0.01f);
    _modelRenderer = modelActor->AddComponent<ModelRenderer>();
	_animator = modelActor->AddComponent<Animator>();

    _targetActor = RegisterActor<Actor>(u8"Target", ActorTag::Stage);
	//_targetActor.lock()->GetTransform().SetPosition(Vector3(5.7f, 2.0f, -0.5f));
	_targetActor.lock()->GetTransform().SetPosition(Vector3(-1.0f, 0.0f, 4.0f));

    // デバッグカメラをオンにする
    Debug::GetDebugInput()->buttonData |= DebugInput::BTN_F4;

    // グリッド表示
    SetShowGrid(true);
}

void SceneWyvernIKDebug::Update(float elapsedTime)
{
    Scene::Update(elapsedTime);
	if (_twoBoneIK)
		UpdateTwoBoneIK(elapsedTime);
	else
		UpdateAnimationIK(elapsedTime);
	if (ImGui::Begin(u8"デバッグ"))
	{
		ImGui::Checkbox(u8"TwoBoneIK", &_twoBoneIK);
		ImGui::Checkbox(u8"IK計算", &_calculateIK);
		ImGui::DragFloat3(u8"ターゲット位置", (float*)&_targetActor.lock()->GetTransform().GetPosition(), 0.1f, -100.0f, 100.0f);
		ImGui::DragFloat3(u8"ポールターゲット位置", &_poleLocalPosition.x, 0.1f, -100.0f, 100.0f);
		ImGui::DragFloat(u8"LERP速度", &_lerpSpeed, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat(u8"LERPタイマー", &_lerpTimer, 0.1f, 0.0f, 10.0f);
		if (ImGui::Checkbox(u8"プレイヤーか", &_playerIK))
		{
			if (_playerIK)
			{
				// プレイヤーのモデルを表示
				_animator.lock()->GetActor()->LoadModel("./Data/Model/Player/ARPG_Warrior.fbx");
				_animator.lock()->ResetModel(_animator.lock()->GetActor()->GetModel().lock());
				_model = _animator.lock()->GetActor()->GetModel();
			}
			else
			{
				// ワイバーンのモデルを表示
				_animator.lock()->GetActor()->LoadModel("./Data/Model/Dragons/Kuzar the Magnificent.fbx");
				_animator.lock()->ResetModel(_animator.lock()->GetActor()->GetModel().lock());
				_model = _animator.lock()->GetActor()->GetModel();
			}
		}
	}
	ImGui::End();
}

void SceneWyvernIKDebug::UpdateTwoBoneIK(float elapsedTime)
{
	Vector3 debugVec3 = Vector3::Zero;

	auto model = _model.lock();
	// 根本、中間、末端のノードを取得
	int rootNodeIndex{},
		midNodeIndex{},
		endNodeIndex{};
	if (_playerIK)
	{
		rootNodeIndex = model->GetNodeIndex("ORG-upper_arm.L");
		midNodeIndex = model->GetNodeIndex("ORG-forearm.L");
		endNodeIndex = model->GetNodeIndex("ORG-hand.L");
	}
	else
	{
		rootNodeIndex = model->GetNodeIndex("L UpperArm");
		midNodeIndex = model->GetNodeIndex("L Forearm");
		endNodeIndex = model->GetNodeIndex("L Hand");
	}
	auto& rootNode = model->GetPoseNodes()[rootNodeIndex];
	auto& midNode = model->GetPoseNodes()[midNodeIndex];
	auto& endNode = model->GetPoseNodes()[endNodeIndex];

	// ポールターゲットをギズモで動かす
	if (_INPUT_IS_PRESSED("Evade"))
	{
		const DirectX::XMFLOAT4X4& view = GetMainCamera()->GetView();
		const DirectX::XMFLOAT4X4& projection = GetMainCamera()->GetProjection();
		DirectX::XMMATRIX MidNodeTransform = DirectX::XMLoadFloat4x4(&midNode.worldTransform);
		DirectX::XMMATRIX PoleLocalTransform = DirectX::XMMatrixTranslation(_poleLocalPosition.x, _poleLocalPosition.y, _poleLocalPosition.z);
		DirectX::XMFLOAT4X4 poleWorldTransform{};
		DirectX::XMStoreFloat4x4(&poleWorldTransform, DirectX::XMMatrixMultiply(PoleLocalTransform, MidNodeTransform));
		Debug::Guizmo(view, projection, &poleWorldTransform, ImGuizmo::TRANSLATE, ImGuizmo::LOCAL);
		Vector3 poleWorldPosition{};
		poleWorldPosition.x = poleWorldTransform._41;
		poleWorldPosition.y = poleWorldTransform._42;
		poleWorldPosition.z = poleWorldTransform._43;
		_poleLocalPosition = Vector3::TransformCoord(poleWorldPosition,
			DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&midNode.worldTransform)));
	}

	// ターゲットのワールド座標取得
	DirectX::XMVECTOR TargetWorldPosition = DirectX::XMLoadFloat3(&_targetActor.lock()->GetTransform().GetPosition());
	// ポールターゲットのワールド座標取得
	DirectX::XMVECTOR PoleWorldPosition = DirectX::XMLoadFloat3(&_poleLocalPosition.TransformCoord(midNode.worldTransform));

	if (_calculateIK)
	{
		// 初期姿勢に戻す
		rootNode.rotation = Quaternion::Identity;
		midNode.rotation = Quaternion::Identity;
		endNode.rotation = Quaternion::Identity;
		model->UpdateNodeTransform(&rootNode);

		// 各ノードのワールド座標取得
		DirectX::XMMATRIX RootTransform = DirectX::XMLoadFloat4x4(&rootNode.worldTransform);
		DirectX::XMMATRIX MidTransform = DirectX::XMLoadFloat4x4(&midNode.worldTransform);
		DirectX::XMMATRIX TipTransform = DirectX::XMLoadFloat4x4(&endNode.worldTransform);
		DirectX::XMVECTOR RootPosition = RootTransform.r[3];
		DirectX::XMVECTOR MidPosition = MidTransform.r[3];
		DirectX::XMVECTOR TipPosition = TipTransform.r[3];

		// 各ノードの長さ算出
		DirectX::XMVECTOR RootToMidle = DirectX::XMVectorSubtract(MidPosition, RootPosition);
		DirectX::XMVECTOR MidleToTip = DirectX::XMVectorSubtract(TipPosition, MidPosition);
		float rootToMidleLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(RootToMidle));
		float midleToTipLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(MidleToTip));
		float boneLength = rootToMidleLength + midleToTipLength;

		// ターゲットまでのベクトルと長さ算出
		DirectX::XMVECTOR RootToTarget = DirectX::XMVectorSubtract(TargetWorldPosition, RootPosition);
		float targetLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(RootToTarget));

		// 根本ノードから中間ノードへのベクトルと根本ノードからターゲットへのベクトルで根本ノードの回転角を作る
		DirectX::XMVECTOR RootToMidleNorm = DirectX::XMVector3Normalize(RootToMidle);
		DirectX::XMVECTOR RootToTargetNorm = DirectX::XMVector3Normalize(RootToTarget);
		DirectX::XMVECTOR RootWorldAxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(RootToMidleNorm, RootToTargetNorm));
		float rootAngle = acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(RootToMidleNorm, RootToTargetNorm)));

		{
			// 求めた回転軸をローカル座標変換
			DirectX::XMVECTOR RootLocalAxis = DirectX::XMVector4Transform(RootWorldAxis, DirectX::XMMatrixInverse(nullptr, RootTransform));
			DirectX::XMVECTOR OldRootLocalRotation = DirectX::XMLoadFloat4(&rootNode.rotation);
			// ローカル回転軸と回転角を使ってQuaternion
			DirectX::XMVECTOR RootLocalQuaternion = DirectX::XMQuaternionMultiply(OldRootLocalRotation, DirectX::XMQuaternionRotationAxis(RootLocalAxis, rootAngle));

			// ノードの長さの総数よりターゲットまでの距離が短ければ
			if (boneLength > targetLength)
			{
				// ヘロンの公式から三角形の面積を求める
				float s = (rootToMidleLength + midleToTipLength + targetLength) / 2;
				float S = sqrtf(s * (s - rootToMidleLength) * (s - midleToTipLength) * (s - targetLength));
				// 面積＊２/ 底辺で高さを求める
				float height = S * 2 / rootToMidleLength;
				// 直角三角形の斜辺と高さから角度を求める
				float angle = asinf(height / targetLength);

				// 根本ノードからポールターゲットへのベクトル
				DirectX::XMVECTOR RootToPoleVec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(PoleWorldPosition, RootPosition));

				// ポールターゲットを利用した回転軸を算出
				RootWorldAxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(RootToMidleNorm, RootToPoleVec));
				RootLocalAxis = DirectX::XMVector4Transform(RootWorldAxis, DirectX::XMMatrixInverse(nullptr, RootTransform));
				// 三角形の面積から算出した角度を反映させる
				RootLocalQuaternion = DirectX::XMQuaternionMultiply(RootLocalQuaternion, DirectX::XMQuaternionRotationAxis(RootLocalAxis, angle));
			}

			DirectX::XMStoreFloat4(&rootNode.rotation, DirectX::XMQuaternionNormalize(RootLocalQuaternion));
			// 自分以下の行列を更新
			model->UpdateNodeTransform(&rootNode);
		}

		// 行列が変化してるので再取得
		RootTransform = DirectX::XMLoadFloat4x4(&rootNode.worldTransform);
		MidTransform = DirectX::XMLoadFloat4x4(&midNode.worldTransform);
		TipTransform = DirectX::XMLoadFloat4x4(&endNode.worldTransform);
		RootPosition = RootTransform.r[3];
		MidPosition = MidTransform.r[3];
		TipPosition = TipTransform.r[3];

		// 中間ノードから先端ノードへのベクトルと中間ノードからターゲットへのベクトルで中間ノードの回転角と回転軸を作る
		DirectX::XMVECTOR MidleToTipVec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(TipPosition, MidPosition));
		DirectX::XMVECTOR MidleToTargetVec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(TargetWorldPosition, MidPosition));
		DirectX::XMVECTOR MidleWorldAxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(MidleToTipVec, MidleToTargetVec));
		float midleAngle = acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(MidleToTargetVec, MidleToTipVec)));

		{
			// 求めた回転軸をローカル座標変換
			DirectX::XMVECTOR MidleLocalAxis = DirectX::XMVector4Transform(MidleWorldAxis, DirectX::XMMatrixInverse(nullptr, MidTransform));
			DirectX::XMVECTOR OldMidleLocalRotation = DirectX::XMLoadFloat4(&midNode.rotation);

			// ローカル回転軸と回転角を使ってQuaternionを作成
			MidleLocalAxis = DirectX::XMQuaternionMultiply(OldMidleLocalRotation, DirectX::XMQuaternionRotationAxis(MidleLocalAxis, midleAngle));
			DirectX::XMStoreFloat4(&midNode.rotation, DirectX::XMQuaternionNormalize(MidleLocalAxis));
			// 自分以下の行列を更新
			model->UpdateNodeTransform(&midNode);
		}
	}

	// デバッグ表示
	DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	{
		Vector3 childWP = Vector3(midNode.worldTransform._41, midNode.worldTransform._42, midNode.worldTransform._43);
		Vector3 parentWP = Vector3(rootNode.worldTransform._41, rootNode.worldTransform._42, rootNode.worldTransform._43);
		float length = Vector3::Length(parentWP - childWP);
		if (length != 0.0f)
		{
			DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&parentWP), DirectX::XMLoadFloat3(&childWP), Up);
			DirectX::XMFLOAT4X4 world;
			DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixInverse(nullptr, View));
			Debug::Renderer::DrawBone(
				world,
				length,
				Vector4::Red
			);
		}
	}
	{
		Vector3 childWP = Vector3(endNode.worldTransform._41, endNode.worldTransform._42, endNode.worldTransform._43);
		Vector3 parentWP = Vector3(midNode.worldTransform._41, midNode.worldTransform._42, midNode.worldTransform._43);
		float length = Vector3::Length(parentWP - childWP);
		if (length != 0.0f)
		{
			DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&parentWP), DirectX::XMLoadFloat3(&childWP), Up);
			DirectX::XMFLOAT4X4 world;
			DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixInverse(nullptr, View));
			Debug::Renderer::DrawBone(
				world,
				length,
				Vector4::Red
			);
		}
	}
	{
		Debug::Renderer::DrawAxis(rootNode.worldTransform);
		Debug::Renderer::DrawAxis(midNode.worldTransform);
		Debug::Renderer::DrawAxis(endNode.worldTransform);
	}
	Debug::Renderer::DrawSphere(_targetActor.lock()->GetTransform().GetPosition(), 0.1f, Vector4::Green);
	Debug::Renderer::DrawSphere(_poleLocalPosition.TransformCoord(midNode.worldTransform), 0.1f, Vector4::Green);

	if (ImGui::Begin(u8"デバッグ"))
	{
		ImGui::DragFloat3(u8"デバッグ", &debugVec3.x, 0.1f, -100.0f, 100.0f);
	}
	ImGui::End();
}

void SceneWyvernIKDebug::UpdateAnimationIK(float elapsedTime)
{
	if (_playerIK)
		return;

	auto model = _model.lock();
	int rootNodeIndex = model->GetNodeIndex("L UpperArm");
	int midNodeIndex = model->GetNodeIndex("L Forearm");
	int endNodeIndex = model->GetNodeIndex("L Hand");
	auto& rootNode = model->GetPoseNodes()[rootNodeIndex];
	auto& midNode = model->GetPoseNodes()[midNodeIndex];
	auto& endNode = model->GetPoseNodes()[endNodeIndex];

	if (!_animator.lock()->IsPlayAnimation())
	{
		_animator.lock()->PlayAnimation(
			u8"AttackWingFistLeft",
			false,
			1.5f);
		_lerpTimer = 0.0f;
	}

	float animationTimer = _animator.lock()->GetAnimationTimer();
	if (animationTimer > 1.7f)
	{
		_lerpTimer -= elapsedTime * _lerpSpeed;
	}
	else if (animationTimer >= 1.2f)
	{
		_lerpTimer += elapsedTime * _lerpSpeed;
	}

	// z : 2.6 ~ 4.0
	// x : 0.0 ~ -1.3
	//  => 半径0.7 中心 (-0.7, 0.0, 3.3)
	// ターゲット位置の制限
	{
		DirectX::XMMATRIX WyvernTransform = DirectX::XMLoadFloat4x4(&_animator.lock()->GetActor()->GetTransform().GetMatrix());
		Vector3 targetPosition = _targetActor.lock()->GetTransform().GetPosition();
		Vector3 targetLocalPosition = targetPosition.TransformCoord(DirectX::XMMatrixInverse(nullptr, WyvernTransform));
		targetLocalPosition = targetLocalPosition.ClampSphere(Vector3(-0.7f * 100.0f, 0.0f, 3.3f * 100.0f), 0.7f * 100.0f);
		_targetActor.lock()->GetTransform().SetPosition(targetLocalPosition.TransformCoord(WyvernTransform));
	}

	if (_calculateIK && _lerpTimer > 0.0f)
	{
		// 現在の姿勢を取得
		Quaternion animationMidRotation = midNode.rotation;
		// 初期姿勢に戻す
		midNode.rotation = Quaternion::Identity;
		model->UpdateNodeTransform(&rootNode);

		// midNodeをターゲット方向に向ける
		DirectX::XMMATRIX MidTransform = DirectX::XMLoadFloat4x4(&midNode.worldTransform);
		DirectX::XMMATRIX EndTransform = DirectX::XMLoadFloat4x4(&endNode.worldTransform);
		DirectX::XMVECTOR TargetWorldPosition = DirectX::XMLoadFloat3(&_targetActor.lock()->GetTransform().GetPosition());
		DirectX::XMVECTOR MidPosition = MidTransform.r[3];
		DirectX::XMVECTOR EndPosition = EndTransform.r[3];
		// 中間ノードからターゲットへのベクトル
		DirectX::XMVECTOR MidleToTargetVec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(TargetWorldPosition, MidPosition));
		// 中間ノードから先端ノードへのベクトル
		DirectX::XMVECTOR MidleToEndVec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(EndPosition, MidPosition));
		// 中間ノードからターゲットへのベクトルと中間ノードから先端ノードへのベクトルで中間ノードの回転角と回転軸を作る
		DirectX::XMVECTOR MidleWorldAxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(MidleToEndVec, MidleToTargetVec));
		float midleAngle = acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(MidleToTargetVec, MidleToEndVec)));
		{
			// 求めた回転軸をローカル座標変換
			DirectX::XMVECTOR MidleLocalAxis = DirectX::XMVector4Transform(MidleWorldAxis, DirectX::XMMatrixInverse(nullptr, MidTransform));
			DirectX::XMVECTOR OldMidleLocalRotation = DirectX::XMLoadFloat4(&Quaternion::Identity);
			// ローカル回転軸と回転角を使ってQuaternionを作成
			MidleLocalAxis = DirectX::XMQuaternionMultiply(OldMidleLocalRotation, DirectX::XMQuaternionRotationAxis(MidleLocalAxis, midleAngle));
			Quaternion nextRotation{};
			DirectX::XMStoreFloat4(&nextRotation, DirectX::XMQuaternionNormalize(MidleLocalAxis));
			// 補間処理
			midNode.rotation = Quaternion::Slerp(
				animationMidRotation,
				nextRotation,
				MathF::Clamp01(_lerpTimer));

			// 自分以下の行列を更新
			model->UpdateNodeTransform(&midNode);
		}
	}

	Debug::Renderer::DrawSphere(_targetActor.lock()->GetTransform().GetPosition(), 0.1f, Vector4::Green);
}
