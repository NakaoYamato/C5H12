#include "SceneTwoBoneIKDebug.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Input/Input.h"
#include <imgui.h>
#include <ImGuizmo.h>

void SceneTwoBoneIKDebug::OnInitialize()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();

	SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
		L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
		L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

	// ボーンデータ初期化
	for (int i = 0; i < _countof(bones); ++i)
	{
		Bone& bone = bones[i];

		if (i == 0)
		{
			bone.localPosition = { 0, 0, 0 };
			bone.localRotation = { 0, 1, 0, 0 };
			bone.parent = nullptr;
			bone.child = &bones[i + 1];
		}
		else
		{
			bone.localPosition = { 0, 0, 2 };
			bone.localRotation = { 0, 0, 0, 1 };
			bone.parent = &bones[i - 1];
			bone.child = (i != _countof(bones) - 1) ? &bones[i + 1] : nullptr;
		}

		// 行列計算
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(bone.localPosition.x, bone.localPosition.y, bone.localPosition.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&bone.localRotation));
		DirectX::XMMATRIX LocalTransform = DirectX::XMMatrixMultiply(R, T);
		DirectX::XMMATRIX ParentWorldTransform = bone.parent != nullptr ? DirectX::XMLoadFloat4x4(&bone.parent->worldTransform) : DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX WorldTransform = DirectX::XMMatrixMultiply(LocalTransform, ParentWorldTransform);
		DirectX::XMStoreFloat4x4(&bone.worldTransform, WorldTransform);
	}
	// ターゲット行列を初期化
	targetWorldTransform = bones[_countof(bones) - 1].worldTransform;

	// ポールターゲット行列を初期化
	Bone& midBone = bones[2];
	DirectX::XMMATRIX MidWorldTransform = DirectX::XMLoadFloat4x4(&midBone.worldTransform);
	DirectX::XMMATRIX PoleLocalTransform = DirectX::XMMatrixTranslation(0, 1, 0);
	DirectX::XMMATRIX PoleWorldTransform = DirectX::XMMatrixMultiply(PoleLocalTransform, MidWorldTransform);
	DirectX::XMStoreFloat4x4(&poleLocalTransform, PoleLocalTransform);
	DirectX::XMStoreFloat4x4(&poleWorldTransform, PoleWorldTransform);

	// デバッグカメラをオンにする
	Debug::GetDebugInput()->buttonData |= DebugInput::BTN_F4;

	// グリッド表示
	SetShowGrid(true);
}

void SceneTwoBoneIKDebug::OnUpdate(float elapsedTime)
{
	// ポールターゲットのワールド行列を更新
	{
		// 中間ボーン付近にポールターゲットを配置する
		Bone& midBone = bones[2];
		DirectX::XMMATRIX MidWorldTransform = DirectX::XMLoadFloat4x4(&midBone.worldTransform);
		DirectX::XMMATRIX PoleLocalTransform = DirectX::XMLoadFloat4x4(&poleLocalTransform);
		DirectX::XMMATRIX PoleWorldTransform = DirectX::XMMatrixMultiply(PoleLocalTransform, MidWorldTransform);
		DirectX::XMStoreFloat4x4(&poleWorldTransform, PoleWorldTransform);
	}
	// ポールターゲットをギズモで動かす
	const DirectX::XMFLOAT4X4& view = GetMainCamera()->GetView();
	const DirectX::XMFLOAT4X4& projection = GetMainCamera()->GetProjection();
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		DirectX::XMMATRIX PoleWorldTransform = DirectX::XMLoadFloat4x4(&poleWorldTransform);
		Debug::Guizmo(
			view, projection, &poleWorldTransform,
			ImGuizmo::TRANSLATE,
			ImGuizmo::LOCAL);
		PoleWorldTransform = DirectX::XMLoadFloat4x4(&poleWorldTransform);

		Bone& midBone = bones[2];
		DirectX::XMMATRIX MidWorldTransform = DirectX::XMLoadFloat4x4(&midBone.worldTransform);
		DirectX::XMMATRIX InverseMidWorldTransform = DirectX::XMMatrixInverse(nullptr, MidWorldTransform);
		DirectX::XMMATRIX PoleLocalTransform = DirectX::XMMatrixMultiply(PoleWorldTransform, InverseMidWorldTransform);
		DirectX::XMStoreFloat4x4(&poleLocalTransform, PoleLocalTransform);
	}
	// ターゲットをギズモで動かす
	else
	{
		Debug::Guizmo(
			view, projection, &targetWorldTransform,
			ImGuizmo::TRANSLATE,
			ImGuizmo::WORLD);
	}

	// ターゲット座標とポール座標を取得
	DirectX::XMMATRIX TargetWorldTransform = DirectX::XMLoadFloat4x4(&targetWorldTransform);
	DirectX::XMVECTOR TargetWorldPosition = TargetWorldTransform.r[3];
	DirectX::XMMATRIX PoleWorldTransform = DirectX::XMLoadFloat4x4(&poleWorldTransform);
	DirectX::XMVECTOR PoleWorldPosition = PoleWorldTransform.r[3];

	// 初期姿勢に戻す
	for (int i = 1; i < _countof(bones); ++i)
	{
		Bone& bone = bones[i];
		bone.localRotation = { 0, 0, 0, 1 };
	}
	bones[0].UpdateWorldTransforms();

	// 3つの関節を使ってIK制御をする
	Bone& rootBone = bones[1];	// 根元ボーン
	Bone& midBone = bones[2];	// 中間ボーン
	Bone& tipBone = bones[3];	// 先端ボーン

	DirectX::XMMATRIX RootBoneTransform = DirectX::XMLoadFloat4x4(&rootBone.worldTransform);
	DirectX::XMMATRIX MidBoneTransform = DirectX::XMLoadFloat4x4(&midBone.worldTransform);
	DirectX::XMMATRIX TipBoneTransform = DirectX::XMLoadFloat4x4(&tipBone.worldTransform);
	DirectX::XMVECTOR RootBonePosition = RootBoneTransform.r[3];
	DirectX::XMVECTOR MidBonePosition = MidBoneTransform.r[3];
	DirectX::XMVECTOR TipBonePosition = TipBoneTransform.r[3];

	DirectX::XMVECTOR RootToMidle = DirectX::XMVectorSubtract(MidBonePosition, RootBonePosition);
	DirectX::XMVECTOR MidleToTip = DirectX::XMVectorSubtract(TipBonePosition, MidBonePosition);
	float rootToMidleLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(RootToMidle));
	float midleToTipLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(MidleToTip));;
	float boneLength = rootToMidleLength + midleToTipLength;
	DirectX::XMVECTOR RootToTarget = DirectX::XMVectorSubtract(TargetWorldPosition, RootBonePosition);
	float targetLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(RootToTarget));

	//根本ボーン
	//根本ボーンから中間ボーンへのベクトルと根本ボーンからターゲットへのベクトルで根本ボーンの回転角を作る
	DirectX::XMVECTOR RootToMidleVec = DirectX::XMVector3Normalize(RootToMidle);
	DirectX::XMVECTOR RootToTargetVec = DirectX::XMVector3Normalize(RootToTarget);
	//DirectX::XMVECTOR RootAxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(RootToTargetVec, RootToMidleVec));
	DirectX::XMVECTOR RootWorldAxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(RootToMidleVec, RootToTargetVec));
	float rootAngle = acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(RootToMidleVec, RootToTargetVec)));

	{
		//求めた回転軸をローカル座標変換
		DirectX::XMVECTOR RootLocalAxis = DirectX::XMVector4Transform(RootWorldAxis, DirectX::XMMatrixInverse(nullptr, RootBoneTransform));
		DirectX::XMVECTOR OldRootLocalRotation = DirectX::XMLoadFloat4(&rootBone.localRotation);
		//ローカル回転軸と回転角を使ってQuaternion
		DirectX::XMVECTOR RootLocalQuaternion = DirectX::XMQuaternionMultiply(OldRootLocalRotation, DirectX::XMQuaternionRotationNormal(RootLocalAxis, rootAngle));


		//ボーンの長さの総数よりターゲットまでの距離が長かったら
		if (boneLength > targetLength)
		{
			//ヘロンの公式から三角形の面積を求める
			float s = (rootToMidleLength + midleToTipLength + targetLength) / 2;
			//float s = (rootToMidleLength + midleToTargetLength + targetLength) / 2;
			float S = sqrtf(s * (s - rootToMidleLength) * (s - midleToTipLength) * (s - targetLength));
			//float S = sqrtf(s * (s - rootToMidleLength) * (s - midleToTargetLength) * (s - targetLength));
			//面積＊２/ 底辺で高さを求める
			float height = S * 2 / rootToMidleLength;
			//直角三角形の斜辺と高さから角度を求める
			float angle = asinf(height / targetLength);


			//根本ボーンからポールターゲットへのベクトル
			DirectX::XMVECTOR RootToPoleVec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(PoleWorldPosition, RootBonePosition));

			//ポールターゲットを利用した回転軸を算出
			RootWorldAxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(RootToMidleVec, RootToPoleVec));
			RootLocalAxis = DirectX::XMVector4Transform(RootWorldAxis, DirectX::XMMatrixInverse(nullptr, RootBoneTransform));
			//三角形の面積から算出した角度を反映させる
			RootLocalQuaternion = DirectX::XMQuaternionMultiply(RootLocalQuaternion, DirectX::XMQuaternionRotationAxis(RootLocalAxis, angle));

		}

		DirectX::XMStoreFloat4(&rootBone.localRotation, DirectX::XMQuaternionNormalize(RootLocalQuaternion));
		//DirectX::XMStoreFloat4(&bones[1].localRotation, RootLocalAxis);
		//自分以下の行列を更新
		rootBone.UpdateWorldTransforms();
	}

	//行列が変化してるので再取得
	RootBoneTransform = DirectX::XMLoadFloat4x4(&rootBone.worldTransform);
	MidBoneTransform = DirectX::XMLoadFloat4x4(&midBone.worldTransform);
	TipBoneTransform = DirectX::XMLoadFloat4x4(&tipBone.worldTransform);
	RootBonePosition = RootBoneTransform.r[3];
	MidBonePosition = MidBoneTransform.r[3];
	TipBonePosition = TipBoneTransform.r[3];

	//中間ボーン
	//中間ボーンから先端ボーンへのベクトルと中間ボーンからターゲットへのベクトルで中間ボーンの回転角と回転軸を作る
	DirectX::XMVECTOR MidleToTipVec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(TipBonePosition, MidBonePosition));
	DirectX::XMVECTOR MidleToTargetVec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(TargetWorldPosition, MidBonePosition));
	DirectX::XMVECTOR MidleWorldAxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(MidleToTipVec, MidleToTargetVec));
	float midleAngle = acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(MidleToTargetVec, MidleToTipVec)));

	{
		//求めた回転軸をローカル座標変換
		DirectX::XMVECTOR MidleLocalAxis = DirectX::XMVector4Transform(MidleWorldAxis, DirectX::XMMatrixInverse(nullptr, MidBoneTransform));
		DirectX::XMVECTOR OldMidleLocalRotation = DirectX::XMLoadFloat4(&midBone.localRotation);

		//ローカル回転軸と回転角を使ってQuaternionを作成
		MidleLocalAxis = DirectX::XMQuaternionMultiply(OldMidleLocalRotation, DirectX::XMQuaternionRotationNormal(MidleLocalAxis, midleAngle));
		//MidleLocalAxis = DirectX::XMQuaternionMultiply(OldMidleLocalRotation, DirectX::XMQuaternionRotationAxis(MidleLocalAxis, midleAngle));
		//MidleLocalAxis = DirectX::XMQuaternionRotationNormal(MidleLocalAxis, midleAngle);
		DirectX::XMStoreFloat4(&midBone.localRotation, DirectX::XMQuaternionNormalize(MidleLocalAxis));
		//自分以下の行列を更新
		midBone.UpdateWorldTransforms();
	}

	// デバッグ表示
	for (size_t i = 0; i < _countof(bones) - 1; ++i)
	{
		const Bone& bone = bones[i];
		const Bone& child = bones[i + 1];

		DirectX::XMFLOAT4X4 world;
		DirectX::XMMATRIX World = DirectX::XMLoadFloat4x4(&bone.worldTransform);
		float length = child.localPosition.z;
		World.r[0] = DirectX::XMVectorScale(DirectX::XMVector3Normalize(World.r[0]), length);
		World.r[1] = DirectX::XMVectorScale(DirectX::XMVector3Normalize(World.r[1]), length);
		World.r[2] = DirectX::XMVectorScale(DirectX::XMVector3Normalize(World.r[2]), length);
		DirectX::XMStoreFloat4x4(&world, World);
		Debug::Renderer::DrawBone(world, length, Vector4::Red);
	}
	Debug::Renderer::DrawSphere(
		Vector3(targetWorldTransform._41, targetWorldTransform._42, targetWorldTransform._43),
		0.1f, Vector4::Green);
	Debug::Renderer::DrawSphere(
		Vector3(poleWorldTransform._41, poleWorldTransform._42, poleWorldTransform._43),
		0.1f, Vector4::Green);
}
