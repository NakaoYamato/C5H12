#pragma once

// 警告の抑制
#pragma warning(push)
#pragma warning(disable:26451)
#pragma warning(disable:26495)
#pragma warning(disable:6385)
#include <Effekseer.h>
#pragma warning(pop)

#include "../Component.h"

class EffekseerEffectController : public Component
{
public:
    EffekseerEffectController(const char* filename) : _effectFileName(filename) {}
	~EffekseerEffectController() override {}
	// 名前取得
	const char* GetName() const override { return "EffekseerEffectController"; }
	// 開始処理
	void Start() override;

    //再生
    Effekseer::Handle Play(const Vector3& position, float scale = 1.0f);
    //停止
    void Stop(Effekseer::Handle handle);
    //座標設定
    void SetPosition(Effekseer::Handle handle, const Vector3& position);
    //スケール設定
    void SetScale(Effekseer::Handle handle, const Vector3& scale);
    // 角度設定
    void SetAngle(Effekseer::Handle handle, const Vector3& angle);
private:
	std::string _effectFileName; // エフェクトファイル名
    Effekseer::EffectRef effekseerEffect;
};