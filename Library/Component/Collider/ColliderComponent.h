#pragma once

#include "../Component.h"

/// <summary>
/// 当たり判定コンポーネント基底クラス
/// </summary>
class ColliderComponent : public Component
{
public:
    /// <summary>
    /// 当たり判定処理
    /// </summary>
    /// <param name="other">当たり判定を行う対象</param>
    /// <param name="collider">当たり判定を行う対象のコンポーネント</param>
    /// <param name="hitPosition">当たったワールド座標</param>
    /// <param name="hitNormal">接触面のotherからthisに向かう法線</param>
    /// <param name="penetration">めり込み量</param>
    /// <returns></returns>
    virtual bool Judge(Actor* other, ColliderComponent* collider,
        Vector3& hitPosition, Vector3& hitNormal, float& penetration) = 0;
};