#include "GrassController.h"

void GrassController::Start()
{
    mr_ = this->GetActor()->GetComponent<ModelRenderer>();
    assert(mr_.lock()->GetShaderName() == "Grass");
}

void GrassController::Update(float elapsedTime)
{
    auto parameter = mr_.lock()->GetShaderParameter();
    auto total = parameter.find("totalElapsedTime");
    if (total != parameter.end())
    {
        parameter["totalElapsedTime"] += elapsedTime;
    }
    mr_.lock()->SetShaderParameter(parameter);
}
