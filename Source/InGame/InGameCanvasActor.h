#pragma once

#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Scene/Scene.h"

#include "../../Library/Component/SpriteRenderer.h"

class InGameCanvasActor : public UIActor
{
public:
	InGameCanvasActor() = default;
	~InGameCanvasActor() override {};

	// ¶¬ˆ—
	void OnCreate() override;
};