#include "CharactorController.h"

#include <imgui.h>

#include "../Scene/Scene.h"

void CharactorController::Update(float elapsedTime)
{
    // 重力適応
    if (_useGravity)
        AddForce(_gravity);

    /// 速度更新
    UpdateVelocity(elapsedTime);
	/// 位置更新
	UpdatePosition(elapsedTime);
	/// 回転更新
	if (_rotateToDirection)
		UpdateRotation(elapsedTime, { _velocity.x, _velocity.z });
}

void CharactorController::FixedUpdate()
{
    /// 位置更新
    //UpdatePosition(_FIXED_UPDATE_RATE);
	/// 回転更新
	//if (_rotateToDirection)
	//	UpdateRotation(_FIXED_UPDATE_RATE, { _velocity.x, _velocity.z });
}

void CharactorController::DrawGui()
{
    ImGui::DragFloat(u8"半径", &_radius, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"スキン幅", &_skinWidth, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"ステップオフセット", &_stepOffset, 0.01f, 0.0f, 100.0f);

	ImGui::Separator();
    ImGui::DragFloat3(u8"加速度", &_acceleration.x, 0.01f);
    ImGui::DragFloat3(u8"速度", &_velocity.x, 0.01f);
    ImGui::DragFloat(u8"最大速度", &_maxSpeedXZ, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"現在の速度", &_currentSpeedXZ, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat3(u8"重力", &_gravity.x, 0.01f);
    ImGui::Checkbox(u8"重力使用", &_useGravity);
    ImGui::Separator();
}

/// 速度更新
void CharactorController::UpdateVelocity(float deltaTime)
{
    // 加速度処理
    _velocity += _acceleration * deltaTime;
    // 最大速度チェック
    _currentSpeedXZ = Vec2Length({ _velocity.x, _velocity.z });
    if (_currentSpeedXZ > _maxSpeedXZ)
    {
        _velocity.x = _velocity.x / _currentSpeedXZ * _maxSpeedXZ;
        _velocity.z = _velocity.z / _currentSpeedXZ * _maxSpeedXZ;
    }
	// 最小速度チェック
	if (_currentSpeedXZ < _minSpeedXZ)
	{
		_velocity.x = 0.0f;
		_velocity.z = 0.0f;
	}

    // 加速力を0
    _acceleration = {};
}

/// 位置更新
void CharactorController::UpdatePosition(float deltaTime)
{
    Vector3 movement = _velocity * deltaTime;
    // 水平処理
    MoveAndSlide({ movement.x, 0.0f, movement.z }, false);
    // 垂直処理
    MoveAndSlide({ 0.0f, movement.y, 0.0f }, true);
}

/// 移動＆滑り
void CharactorController::MoveAndSlide(const Vector3& move, bool vertical)
{
	Transform& transform = this->GetActor()->GetTransform();
	Vector3 calcPosition = transform.GetPosition();
	float calcRadi{};
	Vector3 direction{};
	float distance{};
	Vector3 hitPosition{}, hitNormal{};
	// 計算するための距離
	float moveLength = sqrtf(move.x * move.x + move.y * move.y + move.z * move.z);
	if (moveLength <= FLT_EPSILON)
		return;
	// 計算するため半径
	calcRadi = _radius;

	// Y処理
	if (vertical)
	{
		// 判定を行う円に補正をかける
		calcRadi += _skinWidth;
		direction.y = move.y / moveLength;
		// 計算するための位置補正
		calcPosition.y += _radius + _stepOffset;
		// 足元が球の最小点になるように設定
		distance = _stepOffset + moveLength + (_radius - calcRadi);

		//if (SphereCast(calcPosition, direction, calcRadi, distance, hitPosition, hitNormal))
		//{
		//	// TODO : がくがくする
		//	//Debug::AddVertex(testVertex[0]);
		//	//Debug::AddVertex(testVertex[1]);
		//	//Debug::AddVertex(testVertex[1]);
		//	//Debug::AddVertex(testVertex[2]);
		//	//Debug::AddVertex(testVertex[2]);
		//	//Debug::AddVertex(testVertex[0]);
		//	transform.SetPositionY(calcPosition.y + direction.y * distance - calcRadi);
		//	_velocity.y = 0.0f;
		//}
		//else
		{
			transform.SetPositionY(transform.GetPosition().y + move.y);
		}
	}
	// XZ処理
	else
	{
		// 計算するための位置補正
		calcPosition.y += _radius + _skinWidth + _stepOffset * 0.5f;

		direction.x = move.x / moveLength;
		direction.y = 0.0f;
		direction.z = move.z / moveLength;

		// TODO 01/07エラー
		assert(sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z) - 1.0f < 1e-5);
		distance = moveLength;
		calcRadi += _stepOffset;

		//if (SphereCast(calcPosition, direction, calcRadi, distance, hitPosition, hitNormal))
		//{
		//	//Debug::AddVertex(testVertex[0]);
		//	//Debug::AddVertex(testVertex[1]);
		//	//Debug::AddVertex(testVertex[1]);
		//	//Debug::AddVertex(testVertex[2]);
		//	//Debug::AddVertex(testVertex[2]);
		//	//Debug::AddVertex(testVertex[0]);

		//	// 壁まで移動
		//	transform.SetPositionX(calcPosition.x + direction.x * (distance - _stepOffset));
		//	transform.SetPositionZ(calcPosition.z + direction.z * (distance - _stepOffset));

		//	// 残りの移動量を算出
		//	float remainingMovement = moveLength - distance;

		//	// 残りの移動量があれば壁ずり
		//	if (remainingMovement > 0.0f && distance > 0.0f)
		//	{
		//		// 壁ずり
		//		// レイの向きを残りの移動量倍したベクトルを法線に射影
		//		DirectX::XMVECTOR hitNormalVec = DirectX::XMLoadFloat3(&hitNormal);
		//		DirectX::XMVECTOR remainingDirectionVec = DirectX::XMVector3Normalize(
		//			DirectX::XMLoadFloat3(&direction));
		//		DirectX::XMVectorScale(remainingDirectionVec, remainingMovement);
		//		DirectX::XMFLOAT3 nextDirection{};
		//		{
		//			DirectX::XMVECTOR projection = DirectX::XMVectorScale(
		//				hitNormalVec,
		//				DirectX::XMVectorGetX(
		//					DirectX::XMVector3Dot(remainingDirectionVec,
		//						hitNormalVec)));
		//			DirectX::XMVECTOR nextDirectionVec = DirectX::XMVectorSubtract(remainingDirectionVec, projection);
		//			//DirectX::XMStoreFloat3(&nextMove, nextDirectionVec);
		//			DirectX::XMStoreFloat3(&nextDirection, DirectX::XMVector3Normalize(nextDirectionVec));
		//		}

		//		calcPosition = transform.GetPosition();
		//		calcPosition.y += _radius + _skinWidth + _stepOffset * 0.5f;
		//		distance = remainingMovement;

		//		if (SphereCast(calcPosition, nextDirection, calcRadi, distance, hitPosition, hitNormal))
		//		{
		//			// 壁まで移動
		//			transform.SetPositionX(calcPosition.x + nextDirection.x * (distance - _skinWidth));
		//			transform.SetPositionZ(calcPosition.z + nextDirection.z * (distance - _skinWidth));
		//		}
		//		else
		//		{
		//			transform.SetPositionX(transform.GetPosition().x + nextDirection.x * remainingMovement);
		//			transform.SetPositionZ(transform.GetPosition().z + nextDirection.z * remainingMovement);
		//		}
		//	}
		//}
		//else
		{
			transform.SetPositionX(transform.GetPosition().x + move.x);
			transform.SetPositionZ(transform.GetPosition().z + move.z);
		}
	}
}

void CharactorController::UpdateRotation(float deltaTime, const Vector2& vec)
{
	// 進行ベクトルが0ベクトルの時は処理しない
	if (vec.x == 0.0f && vec.y == 0.0f)
		return;

	Vector3 angle = this->GetActor()->GetTransform().GetRotation();
	float speed = _rotationSpeed * deltaTime;

	// 進行ベクトルを単位ベクトル化
	Vector2 moveVec = Vec2Normalize({ vec.x , vec.y });

	// 自身の回転値から前方向を求める
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	// 回転角を求めるため、２つの単位ベクトルの内積を計算する
	float dot = ((moveVec.x * frontX) + (moveVec.y * frontZ));

	// 内積値は-1.0~1.0で表現されており、２つの単位ベクトルの角度が
	// 小さいほど1.0に近づくという性質を利用して回転速度を調整する
	float rot = 1.0f - dot;
	speed *= rot;

	// 左右判定を行うために２つの単位ベクトルの外積を計算する
	float cross = ((moveVec.x * frontZ) - (moveVec.y * frontX));

	// ２Dの外積値が正の場合か負の場合によって左右判定が行える
	// 左右判定を行うことによって左右回転を選択する
	if (cross < 0.0f)
	{
		this->GetActor()->GetTransform().SetAngleY(angle.y - speed);
	}
	else
	{
		this->GetActor()->GetTransform().SetAngleY(angle.y + speed);
	}
}
