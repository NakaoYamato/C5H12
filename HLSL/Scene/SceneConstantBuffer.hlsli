// シーン定数バッファ
cbuffer SCENE_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 view_projection;
    float4 light_direction;
    float4 light_color;
    float4 camera_position;
    row_major float4x4 inv_view;
    row_major float4x4 inv_projection;
    row_major float4x4 inv_view_projection;
}