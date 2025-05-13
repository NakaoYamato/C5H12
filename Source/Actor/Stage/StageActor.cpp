#include "StageActor.h"

#include "../../Scene/Scene.h"
#include "../../Library/Component/ModelRenderer.h"

// ê∂ê¨éûèàóù
void StageActor::OnCreate()
{
    LoadModel("./Data/Model/Stage/ExampleStage/ExampleStage.fbx");

    //LoadModel("./Data/Model/Stage/BasicTrainingStage/BasicTrainingStage.fbx");
    //GetTransform().SetLengthScale(0.1f);

    //LoadModel("./Data/Model/Stage/Land/Land.fbx");
    //GetTransform().SetLengthScale(0.1f);
    //GetTransform().SetPositionY(-2.7f); 
    
    auto modelCont = AddComponent<ModelRenderer>();

    modelCont->SetRenderType(ModelRenderType::Static);

    AddCollider<MeshCollider>();
}
