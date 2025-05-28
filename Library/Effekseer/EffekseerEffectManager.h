#pragma once

// 警告の抑制
#pragma warning(push)
#pragma warning(disable:26451)
#pragma warning(disable:26495)
#pragma warning(disable:6385)
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#pragma warning(pop)

#include "../../Library/Math/Vector.h"

class EffekseerEffectManager
{
public:
	EffekseerEffectManager();
    ~EffekseerEffectManager() {}

    //更新処理
	void Update(float elapsedTime);
    //描画処理
    void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    //Effeckseerマネージャーの取得
    Effekseer::ManagerRef GetEffekseerManager() { return effekseerManager; }

private:
    Effekseer::ManagerRef           effekseerManager;
    EffekseerRenderer::RendererRef  effekseerRenderer;
};