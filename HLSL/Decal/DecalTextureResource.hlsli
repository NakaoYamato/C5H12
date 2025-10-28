
//  デカールテクスチャ
Texture2D<float4> decaleColorTexture : register(t0);
Texture2D<float4> decaleNormalTexture : register(t1);

Texture2D<float4> gbufferColorMap : register(t2);
Texture2D<float4> gbufferParameterMap : register(t3);
Texture2D<float> gbufferDepthMap : register(t4);
