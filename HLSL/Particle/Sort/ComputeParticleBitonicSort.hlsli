#include "../ComputeParticle.hlsli"
RWStructuredBuffer<ParticleHeader> particleHeaderBuffer : register(u3);

// ”äŠrŠÖ”
bool comparer(in ParticleHeader x0, in ParticleHeader x1)
{
    return (x0.alive > x1.alive || (x0.alive == x1.alive && x0.depth > x1.depth));
}