#include "Easing.h"

#include "../../Library/Algorithm/Converter.h"
#include <imgui.h>

// Gui•`‰æ
void Easings::DrawGui(EasingType& type)
{
    const char* items[] = {
        "InSine",
        "OutSine",
        "InOutSine",
        "InCubic",
        "OutCubic",
        "InOutCubic",
        "InCirc",
        "OutCirc",
        "InOutCirc",
        "InElastic",
        "OutElastic",
        "InOutElastic",
        "InQuad",
        "OutQuad",
        "InOutQuad",
        "InQuart",
        "OutQuart",
        "InOutQuart",
        "InExpo",
        "OutExpo",
        "InOutExpo",
        "InBack",
        "OutBack",
        "InOutBack",
        "InBounce",
        "OutBounce",
        "InOutBounce",
    };
    int currentItem = static_cast<int>(type);
    if (ImGui::Combo("EasingType", &currentItem, items, IM_ARRAYSIZE(items)))
    {
        type = static_cast<EasingType>(currentItem);
    }
}

EasingFunc Easings::GetFunc(EasingType type)
{
    switch (type)
    {
    case EasingType::InSine:
        return Easings::InSine;
    case EasingType::OutSine:
        return Easings::OutSine;
    case EasingType::InOutSine:
        return Easings::InOutSine;
    case EasingType::InCubic:
        return Easings::InCubic;
    case EasingType::OutCubic:
        return Easings::OutCubic;
    case EasingType::InOutCubic:
        return Easings::InOutCubic;
    case EasingType::InCirc:
        return Easings::InCirc;
    case EasingType::OutCirc:
        return Easings::OutCirc;
    case EasingType::InOutCirc:
        return Easings::InOutCirc;
    case EasingType::InElastic:
        return Easings::InElastic;
    case EasingType::OutElastic:
        return Easings::OutElastic;
    case EasingType::InOutElastic:
        return Easings::InOutElastic;
    case EasingType::InQuad:
        return Easings::InQuad;
    case EasingType::OutQuad:
        return Easings::OutQuad;
    case EasingType::InOutQuad:
        return Easings::InOutQuad;
    case EasingType::InQuart:
        return Easings::InQuart;
    case EasingType::OutQuart:
        return Easings::OutQuart;
    case EasingType::InOutQuart:
        return Easings::InOutQuart;
    case EasingType::InExpo:
        return Easings::InExpo;
    case EasingType::OutExpo:
        return Easings::OutExpo;
    case EasingType::InOutExpo:
        return Easings::InOutExpo;
    case EasingType::InBack:
        return Easings::InBack;
    case EasingType::OutBack:
        return Easings::OutBack;
    case EasingType::InOutBack:
        return Easings::InOutBack;
    case EasingType::InBounce:
        return Easings::InBounce;
    case EasingType::OutBounce:
        return Easings::OutBounce;
    case EasingType::InOutBounce:
        return Easings::InOutBounce;
    }
    return nullptr;
}
