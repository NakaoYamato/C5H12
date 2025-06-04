#ifndef __SAMPLER_STATE_DEFINE__
#define __SAMPLER_STATE_DEFINE__

// サンプラーステートのインデックス

static const uint _POINT_WRAP_SAMPLER_INDEX     = 0;
static const uint _POINT_CLAMP_SAMPLER_INDEX    = 1;
static const uint _LINEAR_WRAP_SAMPLER_INDEX    = 2;
static const uint _LINEAR_CLAMP_SAMPLER_INDEX   = 3;
static const uint _BORDER_POINT_SAMPLER_INDEX   = 4;
static const uint _COMPARISION_SAMPLER_INDEX    = 5;
static const uint _ANISOTROPIC_SAMPLER_INDEX    = 6;

static const uint _SAMPLER_STATE_MAX = _ANISOTROPIC_SAMPLER_INDEX + 1;

#endif	//	__SAMPLER_STATE_DEFINE__