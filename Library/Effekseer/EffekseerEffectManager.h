#pragma once

// 警告の抑制
#pragma warning(push)
#pragma warning(disable:26451)
#pragma warning(disable:26495)
#pragma warning(disable:6385)
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#pragma warning(pop)

#include <d3d11.h>

#include "../../Library/Math/Vector.h"

class EffekseerEffectManager
{
public:
    EffekseerEffectManager() {}
    ~EffekseerEffectManager() {}
    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="device"></param>
    /// <param name="dc"></param>
    void Initialize(ID3D11Device* device, ID3D11DeviceContext* dc);
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="elapsedTime"></param>
	void Update(float elapsedTime);
    /// <summary>
    /// 描画処理
    /// </summary>
    /// <param name="view"></param>
    /// <param name="projection"></param>
    void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    //Effeckseerマネージャーの取得
    Effekseer::ManagerRef GetEffekseerManager() { return effekseerManager; }

private:
    Effekseer::ManagerRef           effekseerManager;
    EffekseerRenderer::RendererRef  effekseerRenderer;
};