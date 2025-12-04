#pragma once

#include "../../Library/Component/CameraControllerBase.h"
#include "../../Library/Camera/Camera.h"

class HuntingSuccessCamera : public CameraControllerBase
{
public:
    enum class State
    {
        HorizontalMove,
        Wait,
    };

public:
    HuntingSuccessCamera() {}
    ~HuntingSuccessCamera() override {}
    // 名前取得
    const char* GetName() const override { return "HuntingSuccessCamera"; }
    // 開始処理
    void Start() override;
    // 更新時処理
    void OnUpdate(float elapsedTime) override;
    // GUI描画
    void DrawGui() override;

    // ターゲット設定
    void SetTarget(const Vector3& position, const Vector3& forward, float distance);
	// 次のコントローラー名設定
	void SetNextControllerName(const std::string& name)
	{
		_nextControllerName = name;
	}
private:
    Vector3 _targetPosition{};
    Vector3 _targetForward{ Vector3::Front };
    float _distance = 5.0f;
    float _heightOffset = 2.0f;
    float _eyeOffset = 2.0f;

    State _state = State::HorizontalMove;
    float _timer = 0.0f;

    float _horizontalStartAngle = -DirectX::XMConvertToRadians(15.0f);
    float _horizontalEndAngle = +DirectX::XMConvertToRadians(20.0f);
    float _horizontalTime = 2.0f;

    float _waitTime = 2.0f;
    float _waitDistanceRate = 0.1f;

    // 遷移先
	std::string _nextControllerName{};
};