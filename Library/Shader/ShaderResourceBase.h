#pragma once

#include "../../Library/Resource/ResourceManager.h"
#include "../../Library/Material/Material.h"

class ShaderResourceBase : public ResourceBase
{
public:
	ShaderResourceBase() = default;
	~ShaderResourceBase() override {}

	// シェーダータイプ取得
	virtual ShaderType GetShaderType() const = 0;
};