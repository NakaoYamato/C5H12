#include "GrassController.h"

void GrassController::Start()
{
    _mr = this->GetActor()->GetComponent<ModelRenderer>();
}

void GrassController::Update(float elapsedTime)
{
	if (_mr.lock() == nullptr)
        return;
	if (_mr.lock()->GetMaterials().size() > 0)
        return;
	static std::string PrameterName = "totalElapsedTime";

	auto& materials = _mr.lock()->GetMaterials()[0];
    auto total = materials.GetParameterF1(PrameterName);
    if (total)
    {
		materials.SetParameter(PrameterName, *total + elapsedTime);
    }
}
