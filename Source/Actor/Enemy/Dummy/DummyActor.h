#pragma once

#include "../../Library/Actor/Actor.h"
#include "../../Library/Component/ShapeController.h"

class DummyActor : public Actor
{
public:
    ~DummyActor()override {}
    // ¶¬ˆ—
    void OnCreate() override;
	// XVˆ—
	void OnUpdate(float elapsedTime) override;
	/// <summary>
	/// ÚGˆ—
	/// </summary>
	/// <param name="collisionData">ÚGî•ñ</param>
	void OnContact(CollisionData& collisionData) override;

private:
    std::weak_ptr<ShapeController> _shapeController;
};