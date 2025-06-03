#pragma once
#include <math.h>
#include <cmath>

// ï‚äÆèàóù
inline float EasingLerp(float src, float dst, float t, float(*Easing)(float) = nullptr)
{
    float easingFactor = Easing != nullptr ?
        Easing(t) :
        t;
    return src * (1.0f - easingFactor) + dst * easingFactor;
}

// https://easings.net/ja
class Easings
{
public:
    static constexpr float PI = 3.14159265358979323846f;

public:
    static float InSine(float x)
    {
        return (1 - cosf((x * PI) / 2.0f));
    }
    static float OutSine(float x)
    {
        return (sinf((x * PI) / 2.0f));
    }
    static float InOutSine(float x)
    {
        return -(cosf(PI * x) - 1.0f) / 2.0f;
    }
    static float InCubic(float x)
    {
        return x * x * x;
    }
    static float OutCubic(float x)
    {
        return 1.0f - pow(1.0f - x, 3.0f);
    }
    static float InOutCubic(float x)//5
    {
        return x < 0.5 ? 4.0f * x * x * x :
            1.0f - pow(-2.0f * x + 2.0f, 3.0f) / 2.0f;
    }
    static float InCirc(float x)
    {
        return 1.0f - sqrtf(1.0f - pow(x, 2.0f));
    }
    static float OutCirc(float x)
    {
        return sqrtf(1.0f - pow(x - 1.0f, 2.0f));
    }
    static float InOutCirc(float x)
    {
        return x < 0.5f
            ? (1.0f - sqrtf(1 - pow(2.0f * x, 2.0f))) / 2.0f
            : (sqrtf(1 - pow(-2.0f * x + 2.0f, 2.0f)) + 1.0f) / 2.0f;
    }
    static float InElastic(float x)
    {
        const float c4 = (2.0f * PI) / 3.0f;

        return x == 0.0f
            ? 0.0f
            : x == 1.0f
            ? 1.0f
            : -pow(2.0f, 10.0f * x - 10.0f) *
            sinf((x * 10.0f - 10.750f) * c4);
    }
    static float OutElastic(float x)
    {
        const float c4 = (2.0f * PI) / 3.0f;

        return x == 0.0f
            ? 0.0f
            : x == 1.0f
            ? 1.0f
            : pow(2.0f, -10.0f * x) * sinf((x * 10.0f - 0.75f) * c4) + 1.0f;
    }
    static float InOutElastic(float x)
    {
        const float c5 = (2.0f * PI) / 4.5f;

        return x == 0.0f
            ? 0.0f
            : x == 1.0f
            ? 1.0f
            : x < 0.5f
            ? -(pow(2.0f, 20.0f * x - 10.0f) * sinf((20.0f * x - 11.125f) * c5)) / 2.0f
            : (pow(2.0f, -20.0f * x + 10.0f) * sinf((20.0f * x - 11.125f) * c5)) / 2.0f + 1.0f;
    }
    static float InQuad(float x)
    {
        return x * x;
    }
    static float OutQuad(float x)
    {
        return 1.0f - (1.0f - x) * (1.0f - x);
    }
    static float InOutQuad(float x)
    {
        return x < 0.5f ? 2.0f * x * x : 1.0f - pow(-2.0f * x + 2.0f, 2.0f) / 2.0f;
    }
    static float InQuart(float x)
    {
        return x * x * x * x;
    }
    static float OutQuart(float x)
    {
        return 1.0f - pow(1.0f - x, 4.0f);
    }
    static float InOutQuart(float x)
    {
        return x < 0.5f ? 8.0f * x * x * x * x :
            1.0f - pow(-2.0f * x + 2.0f, 4.0f) / 2.0f;
    }
    static float InExpo(float x)
    {
        return x == 0.0f ? 0.0f : pow(2.0f, 10.0f * x - 10.0f);
    }
    static float OutExpo(float x)
    {
        return x == 1.0f ? 1.0f : 1.0f - pow(2.0f, -10.0f * x);
    }
    static float InOutExpo(float x)
    {
        return x == 0.0f
            ? 0.0f
            : x == 1.0f
            ? 1.0f
            : x < 0.5f ? pow(2.0f, 20.0f * x - 10.0f) / 2.0f
            : (2.0f - pow(2.0f, -20.0f * x + 10.0f)) / 2.0f;
    }
    static float InBack(float x)
    {
        const float c1 = 1.70158f;
        const float c3 = c1 + 1.0f;

        return c3 * x * x * x - c1 * x * x;
    }
    static float OutBack(float x)
    {
        const float c1 = 1.70158f;
        const float c3 = c1 + 1.0f;

        return 1 + c3 * pow(x - 1.0f, 3.0f) + c1 * pow(x - 1.0f, 2.0f);
    }
    static float InOutBack(float x)
    {
        const float c1 = 1.70158f;
        const float c2 = c1 * 1.525f;

        return x < 0.5f
            ? (pow(2.0f * x, 2.0f) * ((c2 + 1.0f) * 2.0f * x - c2)) / 2.0f
            : (pow(2.0f * x - 2.0f, 2.0f) * ((c2 + 1.0f) * (x * 2.0f - 2.0f)
                + c2) + 2.0f) / 2.0f;
    }
    static float InBounce(float x)
    {
        float X = 1.0f - x;
        float ret = 1.0f - OutBounce(X);
        x = 1.0f - X;
        return ret;
    }
    static float OutBounce(float& x)
    {
        const float n1 = 7.5625f;
        const float d1 = 2.75f;

        if (x < 1.0f / d1) {
            return n1 * x * x;
        }
        else if (x < 2.0f / d1) {
            return n1 * (x -= 1.5f / d1) * x + 0.75f;
        }
        else if (x < 2.5 / d1) {
            return n1 * (x -= 2.25f / d1) * x + 0.9375f;
        }
        else {
            return n1 * (x -= 2.625f / d1) * x + 0.984375f;
        }
    }
    static float InOutBounce(float& x)
    {
        if (x < 0.5f)
        {
            float X = 1.0f - 2.0f * x;
            float ret = (1.0f - OutBounce(X)) / 2.0f;
            x = (1.0f - X) / 2.0f;
            return ret;
        }
        else
        {
            float X = 2.0f * x - 1.0f;
            float ret = (1.0f + OutBounce(X)) / 2.0f;
            x = (X + 1.0f) / 2.0f;
            return ret;
        }
    }
};