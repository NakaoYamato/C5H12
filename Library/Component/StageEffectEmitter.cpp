#include "StageEffectEmitter.h"

#include <imgui.h>

// 開始時処理
void StageEffectEmitter::Start()
{
	_effectController = GetActor()->GetComponent<EffectController>();
}

// GUI描画
void StageEffectEmitter::DrawGui()
{
	ImGui::DragInt("Effect Index", &_effectIndex);
}

// オブジェクトとの接触した瞬間時の処理
void StageEffectEmitter::OnContactEnter(CollisionData& collisionData)
{
	if (_effectController.lock() && collisionData.otherLayer == CollisionLayer::Effect)
	{
		_effectController.lock()->Play(
			_effectIndex,
			collisionData.hitPosition);

		// カメラシェイク
		if (auto cameraActor = GetActor()->GetScene()->GetMainCameraActor())
		{
			CameraShakeInfo shake;
			shake.amplitude = _amplitude;
			shake.frequency = _frequency;
			shake.duration = _duration;
			shake.directionMask = _directionMask;
			shake.decayType = _decayType;
			cameraActor->GetShakeManager()->StartShake(shake);
		}
	}
}
