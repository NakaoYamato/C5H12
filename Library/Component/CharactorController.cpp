#include "CharactorController.h"

#include <imgui.h>

#include "../Scene/Scene.h"
#include "../../DebugSupporter/DebugSupporter.h"

// 初期化処理
void CharactorController::Start()
{
	// コンポーネント取得
	_animator = GetActor()->GetComponent<Animator>();
}

// 更新処理
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
	/// ルートモーションによる回転更新
	if (_rotateToDirectionByRootMotion)
		UpdateRotation(elapsedTime, { _animator.lock()->GetRootMovement().x, _animator.lock()->GetRootMovement().z });
}

// 遅延更新処理
void CharactorController::LateUpdate(float elapsedTime)
{
	/// めり込みの解消処理
	ResolvPushOut();
}

void CharactorController::FixedUpdate()
{
    /// 位置更新
    //UpdatePosition(_FIXED_UPDATE_RATE);
	/// 回転更新
	//if (_rotateToDirection)
	//	UpdateRotation(_FIXED_UPDATE_RATE, { _velocity.x, _velocity.z });
}

// デバッグ描画処理
void CharactorController::DebugRender(const RenderContext& rc)
{
	Debug::Renderer::DrawSphere(
		GetActor()->GetTransform().GetPosition() + Vector3(0.0f, _radius + _stepOffset, 0.0f),
		_radius,
		Vector4::Yellow);
}

void CharactorController::DrawGui()
{
    ImGui::DragFloat(u8"半径", &_radius, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"スキン幅", &_skinWidth, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"ステップオフセット", &_stepOffset, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"スロープ", &_slopeLimit, 0.01f, 0.0f, DirectX::XM_PI);
    ImGui::DragFloat(u8"質量", &_mass, 0.01f, 0.0f, 100.0f);

	ImGui::Separator();
    ImGui::DragFloat3(u8"加速度", &_acceleration.x, 0.01f);
    ImGui::DragFloat3(u8"速度", &_velocity.x, 0.01f);
    ImGui::DragFloat(u8"最大速度", &_maxSpeedXZ, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"現在の速度", &_currentSpeedXZ, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat3(u8"重力", &_gravity.x, 0.01f);
    ImGui::Checkbox(u8"重力使用", &_useGravity);
    ImGui::Separator();
}

// 接触の解消処理
void CharactorController::OnContact(CollisionData& collisionData)
{   
	// トリガーでなければ押し出し処理
    if (!collisionData.isTrigger && !collisionData.otherIsTrigger)
    {
		auto otherCharactor = collisionData.other->GetComponent<CharactorController>();
		if (otherCharactor)
		{
            float sum = _mass + otherCharactor->GetMass();
			// 押し出し量を取得　重みを考慮
			if (sum != 0.0f)
				_pushOut += (otherCharactor->GetMass() / sum) * collisionData.penetration * collisionData.hitNormal;
			else	
				_pushOut += collisionData.penetration * collisionData.hitNormal;
		}
    }
}

/// 速度更新
void CharactorController::UpdateVelocity(float deltaTime)
{
    // 加速度処理
    _velocity += _acceleration * deltaTime;
    // 最大速度チェック
    _currentSpeedXZ = Vector2::Length({ _velocity.x, _velocity.z });
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
	// ルートモーションの移動量取得
	if (_animator.lock())
		movement += _animator.lock()->GetRootMovement();

    // 水平処理
    MoveAndSlide({ movement.x, 0.0f, movement.z }, false);
    // 垂直処理
    MoveAndSlide({ 0.0f, movement.y, 0.0f }, true);

	// 押し出し量をクリア
	_pushOut = {};
}

/// 移動＆滑り
void CharactorController::MoveAndSlide(const Vector3& move, bool vertical)
{
	CollisionManager& collisionManager = this->GetActor()->GetScene()->GetCollisionManager();
	Transform& transform = this->GetActor()->GetTransform();

	DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&transform.GetPosition());
	DirectX::XMVECTOR Move = DirectX::XMLoadFloat3(&move);
	DirectX::XMVECTOR CenterOffset = DirectX::XMVectorSet(0, _radius + _stepOffset, 0, 0);

	for (int i = 0; i < 3; ++i)
	{
		// キャスト量算出
		float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(Move));
		if (distance <= FLT_EPSILON)
			break;

		distance += _skinWidth;

		// 縦移動時はステップオフセット分もキャスト量を増やす
		if (vertical)
			distance += _stepOffset;

		DirectX::XMVECTOR Origine = DirectX::XMVectorAdd(Position, CenterOffset);
		DirectX::XMVECTOR Direction = DirectX::XMVector3Normalize(Move);
		Vector3 origine{}, direction{};
		DirectX::XMStoreFloat3(&origine, Origine);
		DirectX::XMStoreFloat3(&direction, Direction);
		Vector3 hitPosition{}, hitNormal{};
		// スフィアキャスト
		if (collisionManager.SphereCast(origine, direction, _radius, distance, hitPosition, hitNormal))
		{
			// 増やしたキャスト量を引く
			distance -= _skinWidth;
			if (vertical)
				distance -= _stepOffset;

			// 移動量を計算
			DirectX::XMVECTOR Vec = DirectX::XMVectorScale(Direction, distance);

			// 移動量を適応
			Position = DirectX::XMVectorAdd(Position, Vec);

			// 法線ベクトル
			DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hitNormal);

			// スロープ
			if (vertical)
			{
				// 速度を0にする
				_velocity.y = 0.0f;
				// スロープの角度を計算
				DirectX::XMVECTOR Up = DirectX::XMVectorSet(0, 1, 0, 0);
				float angle = acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(Normal, Up)));
				// 制限角度以内なら滑る処理をskip
				if (angle < _slopeLimit)
				{
					break;
				}
			}

			// 移動した量を減らす
			Move = DirectX::XMVectorSubtract(Move, Vec);

			// 移動ベクトルを壁に沿わせる
			DirectX::XMVECTOR Projection = DirectX::XMVectorScale(
				Normal,
				DirectX::XMVectorGetX(DirectX::XMVector3Dot(Move, Normal))
			);

			// 壁に沿ったベクトルを計算 (Move から法線方向の成分を引く)
			Move = DirectX::XMVectorSubtract(Move, Projection);
		}
		else
		{
			Position = DirectX::XMVectorAdd(Position, Move);
			break;
		}
	}

	Vector3 position{};
	DirectX::XMStoreFloat3(&position, Position);
	transform.SetPosition(position);
}

void CharactorController::UpdateRotation(float deltaTime, const Vector2& vec)
{
	// 進行ベクトルが0ベクトルの時は処理しない
	if (vec.x == 0.0f && vec.y == 0.0f)
		return;

	Vector3 angle = this->GetActor()->GetTransform().GetRotation();
	float speed = _rotationSpeed * deltaTime;

	// 進行ベクトルを単位ベクトル化
	Vector2 moveVec = Vector2::Normalize({ vec.x , vec.y });

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

/// めり込みの解消処理
void CharactorController::ResolvPushOut()
{
	if (_pushOut == Vector3::Zero)
		return;

	// スフィアキャストでめり込み解消
	MoveAndSlide({ _pushOut.x, 0.0f, _pushOut.z }, false);
	// 垂直処理
	MoveAndSlide({ 0.0f, _pushOut.y, 0.0f }, true);

	_pushOut = {};
}
