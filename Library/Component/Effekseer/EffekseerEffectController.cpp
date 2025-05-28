#include "EffekseerEffectController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"

#include <imgui.h>

// 開始処理
void EffekseerEffectController::Start()
{
    //エフェクトを読み込みする前にロックする
    //※マルチスレッドでEffectを作成するとDeviceContextを同時アクセスして
    //　フリーズする可能性があるので排他制御する
    std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());

    //Effekseerのリソースを読み込む
    //EffekseerはUTF-16のファイルパス以外は対応していないため文字コード変換が必要
    char16_t utf16Filename[256];
    Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, _effectFileName.c_str());

    //Effekseer::Managerを取得
    Effekseer::ManagerRef effekseerManager = GetActor()->GetScene()->GetEffekseerEffectManager()->GetEffekseerManager();

    //Effekseerエフェクトを読み込み
    effekseerEffect = Effekseer::Effect::Create(effekseerManager, (EFK_CHAR*)utf16Filename);
}
//再生
Effekseer::Handle EffekseerEffectController::Play(const Vector3& position, float scale)
{
    Effekseer::ManagerRef effekseerManager = GetActor()->GetScene()->GetEffekseerEffectManager()->GetEffekseerManager();

    Effekseer::Handle handle = effekseerManager->Play(effekseerEffect, position.x, position.y, position.z);

    effekseerManager->SetScale(handle, scale, scale, scale);
    return handle;
}
//停止
void EffekseerEffectController::Stop(Effekseer::Handle handle)
{
    Effekseer::ManagerRef effekseerManager = GetActor()->GetScene()->GetEffekseerEffectManager()->GetEffekseerManager();
    effekseerManager->StopEffect(handle);
}
//座標設定
void EffekseerEffectController::SetPosition(Effekseer::Handle handle, const Vector3& position)
{
    Effekseer::ManagerRef effekseerManager = GetActor()->GetScene()->GetEffekseerEffectManager()->GetEffekseerManager();

    effekseerManager->SetLocation(handle, position.x, position.y, position.z);
}
//スケール設定
void EffekseerEffectController::SetScale(Effekseer::Handle handle, const Vector3& scale)
{
    Effekseer::ManagerRef effekseerManager = GetActor()->GetScene()->GetEffekseerEffectManager()->GetEffekseerManager();

    effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
}
// 角度設定
void EffekseerEffectController::SetAngle(Effekseer::Handle handle, const Vector3& angle)
{
    Effekseer::ManagerRef effekseerManager = GetActor()->GetScene()->GetEffekseerEffectManager()->GetEffekseerManager();

    effekseerManager->SetRotation(handle, angle.x, angle.y, angle.z);
}
