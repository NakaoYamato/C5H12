#include "AnimatorCamera.h"

#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始処理
void AnimatorCamera::Start()
{
	_animator = GetActor()->GetComponent<Animator>();
}

// 更新処理
void AnimatorCamera::Update(float elapsedTime)
{
	auto animator = _animator.lock();
	if (!animator) return;
	auto mainCamera = GetActor()->GetScene()->GetMainCameraActor();
	if (!mainCamera) return;


	auto& animationEvent = animator->GetAnimationEvent();
	auto& massageList = animationEvent.GetMessageList();
	int massageListSize = (int)animationEvent.GetMessageList().size();
	auto events = animator->GetCurrentEvents();
	for (auto& event : events)
	{
		// メッセージインデックスが範囲外ならcontinue
		if (event.messageIndex < 0 || event.messageIndex >= massageListSize)
			continue;

		if (massageList.at(event.messageIndex) == "CameraEyeOffsetStart")
		{
			float rate = (animator->GetAnimationTimer() - event.startSeconds) / (event.endSeconds - event.startSeconds);
			Vector3 offset = event.position;
			mainCamera->AddEyeOffset(offset * rate);
		}
		if (massageList.at(event.messageIndex) == "CameraEyeOffsetEnd")
		{
			float rate = 1.0f - (animator->GetAnimationTimer() - event.startSeconds) / (event.endSeconds - event.startSeconds);
			Vector3 offset = event.position;
			mainCamera->AddEyeOffset(offset * rate);
		}
		if (massageList.at(event.messageIndex) == "CameraEyeOffsetLoop")
		{
			mainCamera->AddEyeOffset(event.position);
		}
	}
}

// GUI描画
void AnimatorCamera::DrawGui()
{
}
