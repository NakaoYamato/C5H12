#include "TerrainShadow.hlsli"

CSM_HS_IN main(CSM_VS_IN vin)
{
    // テッセレーションを行うため頂点情報とインスタンス番号をそのまま出力
    CSM_HS_IN vout  = (CSM_HS_IN) 0;
    vout.position   = vin.position;
    vout.instanceId = vin.instanceId;
    return vout;
}
