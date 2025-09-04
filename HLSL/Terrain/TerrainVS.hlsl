#include "Terrain.hlsli"

HS_IN main(VS_IN vin)
{
    // テッセレーションを行うため頂点情報をそのまま出力
    HS_IN vout = (HS_IN) 0;
    vout.position = vin.position;
    return vout;
}