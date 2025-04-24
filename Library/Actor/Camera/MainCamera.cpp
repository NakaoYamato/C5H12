#include "MainCamera.h"
#include "../../Library/Graphics/Graphics.h"

#include "../../Scene/Scene.h"
// 生成時処理
void MainCamera::OnCreate()
{
	Actor::OnCreate();

	// カメラの初期化
	SetLookAt(
		Vector3(0.0f, 10.0f, -10.0f),
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3(0.0f, 1.0f, 0.0f)
	);
    float screenWidth = Graphics::Instance().GetScreenWidth();
    float screenHeight = Graphics::Instance().GetScreenHeight();
	SetPerspectiveFov(
        DirectX::XMConvertToRadians(50),	// 画角
        screenWidth / screenHeight,			// 画面アスペクト比
        0.1f,								// ニアクリップ
        1000.0f								// ファークリップ
	);
}

// 開始関数
void MainCamera::Start()
{
	Actor::Start();
}

// 更新処理
void MainCamera::Update(float elapsedTime)
{
	Actor::Update(elapsedTime);
}

// GUI描画
void MainCamera::DrawGui()
{
	Actor::DrawGui();
}

void MainCamera::SetLookAt(const Vector3& eye, const Vector3& focus, const Vector3& up)
{
	GetScene()->GetMainCamera()->SetLookAt(eye, focus, up);
    // アクターのトランスフォームに適応
	_transform.SetPosition(eye);
    DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&GetScene()->GetMainCamera()->GetView()));
    DirectX::XMVECTOR S, R, T;
    DirectX::XMMatrixDecompose(&S, &R, &T, World);
    Vector3 s, r, t;
    r = QuaternionToRollPitchYaw(R);
    _transform.SetAngle(r);
}

void MainCamera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
	GetScene()->GetMainCamera()->SetPerspectiveFov(fovY, aspect, nearZ, farZ);
}
