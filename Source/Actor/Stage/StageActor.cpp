#include "StageActor.h"

#include "../../Scene/Scene.h"
#include "../../Library/Component/ModelRenderer.h"

// ê∂ê¨éûèàóù
void StageActor::OnCreate()
{
	Actor::OnCreate();

    LoadModel("./Data/Model/Stage/Land/Land.fbx");
    GetTransform().SetPositionY(-2.7f);
    GetTransform().SetLengthScale(0.1f);
    auto modelCont = AddComponent<ModelRenderer>();

    modelCont->SetRenderType(ModelRenderType::Static);
}
