#include "CameraEventReceiver.h"

#include <imgui.h>

void CameraEventReceiver::Update(float elapsedTime)
{
	_eyeOffset = Vector3::Zero;

	for (auto it = _onUpdateCallbacks.begin(); it != _onUpdateCallbacks.end(); )
	{
		if ((*it)(elapsedTime, this))
		{
			it = _onUpdateCallbacks.erase(it);
		}
		else
		{
			++it;
		}
	}
}

// GUI描画
void CameraEventReceiver::DrawGui()
{
	if (ImGui::Button("Test1"))
	{
		static float TestTimer = 0.0f;
		std::function<bool(float, CameraEventReceiver*)> testCallback =
			[](float elapsedTime, CameraEventReceiver* receiver) -> bool
			{
				receiver->AddEyeOffset(Vector3(0.0f, 0.0f, -5.0f) * TestTimer);

				TestTimer += elapsedTime;
				if (TestTimer > 1.0f)
				{
					TestTimer = 0.0f;
					return true;
				}
				return false;
			};
		this->AddOnUpdateCallback(testCallback);
	}
	if (ImGui::Button("Test2"))
	{
		static float TestTimer = 0.0f;
		std::function<bool(float, CameraEventReceiver*)> testCallback =
			[](float elapsedTime, CameraEventReceiver* receiver) -> bool
			{
				receiver->AddEyeOffset(Vector3(0.0f, 0.0f, -5.0f));
				return true;
			};
		this->AddOnUpdateCallback(testCallback);
	}
	ImGui::Text(u8"設定されている関数:%d", _onUpdateCallbacks.size());
	ImGui::DragFloat3(u8"目のオフセット", &_eyeOffset.x, 0.1f);
}
