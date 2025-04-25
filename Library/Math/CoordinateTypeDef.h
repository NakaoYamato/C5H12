#pragma once

#include <DirectXMath.h>

// ç¿ïWån
enum CoordinateType : int
{
	RHS_Y_UP = 0,
	LHS_Y_UP = 1,
	RHS_Z_UP = 2,
	LHS_Z_UP = 3,
};

static const DirectX::XMFLOAT4X4 COORDINATE_SYSTEM_TRANSFORMS[]
{
	{
		-1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	},
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	},
	{
		-1,0,0,0,
		0,0,-1,0,
		0,1,0,0,
		0,0,0,1
	},
	{
		-1,0,0,0,
		0,0,1,0,
		0,1,0,0,
		0,0,0,1
	},
};
