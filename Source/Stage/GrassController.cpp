#include "GrassController.h"

void GrassController::Start()
{
    _mr = this->GetActor()->GetComponent<ModelRenderer>();
}

void GrassController::Update(float elapsedTime)
{
    auto parameter = _mr.lock()->GetShaderParameter();
    auto total = parameter.find("totalElapsedTime");
    if (total != parameter.end())
    {
        parameter["totalElapsedTime"] += elapsedTime;
    }
    _mr.lock()->SetShaderParameter(parameter);
}
