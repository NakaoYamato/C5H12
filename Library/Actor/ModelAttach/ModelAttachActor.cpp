#include "ModelAttachActor.h"

#include "../../Library/Scene/Scene.h"

// ギズモ描画
void ModelAttachActor::DrawGuizmo()
{
	Matrix4X4 transform = _transform.GetMatrix();
	if (Debug::Guizmo(GetScene()->GetMainCamera()->GetView(), GetScene()->GetMainCamera()->GetProjection(),
		&transform))
	{
		// 単位を考慮した行列から位置、回転、スケールを取得
		DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&transform);
		DirectX::XMMATRIX C{ DirectX::XMMatrixScaling(_transform.GetLengthScale(), _transform.GetLengthScale(),_transform.GetLengthScale()) };
		DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&_parentNode->worldTransform);
		M = DirectX::XMMatrixInverse(nullptr, C) * M * DirectX::XMMatrixInverse(nullptr, P);
		DirectX::XMVECTOR S, R, T;
		DirectX::XMMatrixDecompose(&S, &R, &T, M);
		Vector3 s, r, t;
		DirectX::XMStoreFloat3(&s, S);
		DirectX::XMStoreFloat3(&t, T);
		r = Quaternion::ToRollPitchYaw(R);
		_transform.SetPosition(t);
		_transform.SetScale(s);
		_transform.SetAngle(r);
	}
}
