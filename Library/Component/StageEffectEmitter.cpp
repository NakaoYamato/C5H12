#include "StageEffectEmitter.h"

#include <imgui.h>

// 開始時処理
void StageEffectEmitter::Start()
{
	_effectController = GetActor()->GetComponent<EffectController>();
	if (_effectController.lock())
		_effectStartScale = _effectController.lock()->GetEffectData(static_cast<UINT>(_effectIndex))->GetScale();
}

// GUI描画
void StageEffectEmitter::DrawGui()
{
	ImGui::DragInt("Effect Index", &_effectIndex);
}

// エフェクト再生
void StageEffectEmitter::PlayEffect(const Vector3& position, const Vector3& angle, float effectScale, float shakePower)
{
	if (_effectController.lock())
	{
		_effectController.lock()->Play(_effectIndex, position, angle, _effectStartScale * effectScale);

		// カメラシェイク
		if (auto cameraActor = GetActor()->GetScene()->GetMainCameraActor())
		{
			CameraShakeInfo shake;
			shake.amplitude = _amplitude *shakePower;
			shake.frequency = _frequency;
			shake.duration = _duration;
			shake.directionMask = _directionMask;
			shake.decayType = _decayType;
			cameraActor->GetShakeManager()->StartShake(shake);
		}
	}
}
