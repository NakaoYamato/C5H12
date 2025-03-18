#include "../../Sprite/Sprite.hlsli"
#define FXAA_PC 1
#define FXAA_HLSL_5 1
#define FXAA_QUALITY__PRESET 12
#define FXAA_GREEN_AS_LUMA 1
#include "FXAA.hlsl"

cbuffer CONSTANT_BUFFER : register(b1)
{
    // 画面の大きさの逆数
    float2 invScreenSize;
    
    // エイリアス除去量
    //   1.00 - upper limit (softer)
    //   0.75 - default amount of filtering
    //   0.50 - lower limit (sharper, less sub-pixel aliasing removal)
    //   0.25 - almost off
    //   0.00 - completely off
    float subpix;
    
    // エッジしきい値
    //   0.333 - too little (faster)
    //   0.250 - low quality
    //   0.166 - default
    //   0.125 - high quality 
    //   0.063 - overkill (slower)
    float edgeThreshold;
    
    // エッジしきい値の下限
    //   0.0833 - upper limit (default, the start of visible unfiltered edges)
    //   0.0625 - high quality (faster)
    //   0.0312 - visible limit (slower)
    float edgeThresholdMin;
    
    float3 dummy;
}

Texture2D texture0 : register(t0);
#include "../../Define/SamplerStateDefine.hlsli"
SamplerState samplerStates[_SAMPLER_STATE_MAX] : register(s0);

float4 main(VsOut pin) : SV_TARGET
{
    FxaaTex InputFXAATex = { samplerStates[_LINEAR_CLAMP_SAMPLER_INDEX], texture0 };
    return FxaaPixelShader(
        pin.texcoord,                       // FxaaFloat2 pos,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), // FxaaFloat4 fxaaConsolePosPos,
        InputFXAATex,                       // FxaaTex tex,
        InputFXAATex,                       // FxaaTex fxaaConsole360TexExpBiasNegOne,
        InputFXAATex,                       // FxaaTex fxaaConsole360TexExpBiasNegTwo,
        invScreenSize,                      // FxaaFloat2 fxaaQualityRcpFrame,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), // FxaaFloat4 fxaaConsoleRcpFrameOpt,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), // FxaaFloat4 fxaaConsoleRcpFrameOpt2,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), // FxaaFloat4 fxaaConsole360RcpFrameOpt2,
        subpix, // FxaaFloat fxaaQualitySubpix,
        edgeThreshold, // FxaaFloat fxaaQualityEdgeThreshold,
        edgeThresholdMin, // FxaaFloat fxaaQualityEdgeThresholdMin,
        0.0f, // FxaaFloat fxaaConsoleEdgeSharpness,
        0.0f, // FxaaFloat fxaaConsoleEdgeThreshold,
        0.0f, // FxaaFloat fxaaConsoleEdgeThresholdMin,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f) // FxaaFloat fxaaConsole360ConstDir,
    );
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}