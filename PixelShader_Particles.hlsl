#include"ShaderIncludes.hlsli"

Texture2D ParticleMap : register(t0);
SamplerState BasicSampler : register(s0);


struct VToP_Particle
{
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

float4 main(VToP_Particle input) : SV_TARGET
{
    //basic color sample from texture
    float4 finalColor;
    finalColor = ParticleMap.Sample(BasicSampler, input.uv);
    finalColor *= input.color;
    return finalColor;
}