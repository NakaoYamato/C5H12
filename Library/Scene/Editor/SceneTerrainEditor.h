#pragma once

#include "../SceneManager.h"

class SceneTerrainEditor : public Scene
{
public:
    SceneTerrainEditor() {}
    ~SceneTerrainEditor() override {}
    // –¼‘Oæ“¾
    const char* GetName() const { return u8"TerrainEditor"; }
    // ŠK‘wæ“¾
    SceneMenuLevel GetLevel() const { return SceneMenuLevel::Editor; }
    // ‰Šú‰»
    void OnInitialize() override;
    // ©g‚ğV‹K‚Ìƒ|ƒCƒ“ƒ^‚Å“n‚·
    std::shared_ptr<Scene> GetNewShared() override
    {
        return std::make_shared<SceneTerrainEditor>();
    }
};

// ƒƒjƒ…[ƒo[‚É“o˜^
_REGISTER_SCENE(SceneTerrainEditor)