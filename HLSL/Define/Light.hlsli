struct PointLight
{
    // 位置
    float4 position;
    // 色
    float4 color;
    
    // 範囲
    float range;
    // 使用フラグ 1 使用
    int isAlive;
    float2 dummy;
};