#include "StageActor.h"

#include "../../Scene/Scene.h"
#include "../../Library/Component/ModelRenderer.h"

#include "../../Library/Component/Terrain/TerrainController.h"
#include "../../Library/Component/Terrain/TerrainDeformer.h"
#include "../../Library/Component/Terrain/TerrainCollider.h"

// ê∂ê¨éûèàóù
void StageActor::OnCreate()
{
#if 0
    //LoadModel("./Data/Model/Stage/Ground/ground.fbx");
    //GetTransform().SetPositionY(-5.0f);

    //LoadModel("./Data/Model/Stage/BasicTrainingStage/BasicTrainingStage.fbx");
    //GetTransform().SetLengthScale(0.01f);

    LoadModel("./Data/Model/Stage/Plane/Plane.fbx");
    GetTransform().SetLengthScale(0.1f);
    //GetTransform().SetPositionY(-2.7f); 

    auto modelCont = AddComponent<ModelRenderer>();

    modelCont->SetRenderType(ModelRenderType::Static);
    //modelCont->SetColor(Vector4::Gray);

    AddCollider<MeshCollider>();
#else
    GetTransform().SetScale(50.0f);
    GetTransform().UpdateTransform(nullptr);

    auto terrainController = this->AddComponent<TerrainController>(_terrainPath);
    this->AddComponent<TerrainDeformer>();
    this->AddCollider<TerrainCollider>();
#endif
}
