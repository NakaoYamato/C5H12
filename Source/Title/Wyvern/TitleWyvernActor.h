#pragma once

#include "../../Library/Actor/Actor.h"

#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"

class TitleWyvernActor : public Actor
{
public:
    TitleWyvernActor() {}
    ~TitleWyvernActor() override {}
    // ¶¬ˆ—
    void OnCreate() override;
    // ŠJnˆ—
    void OnStart() override;

private:
    std::weak_ptr<Animator> _animator;
};