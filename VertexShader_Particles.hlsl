#include"ShaderIncludes.hlsli"

struct VToP_Particle
{
    float4 screenPosition : SV_POSITION; // XYZW position (System Value Position)
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

struct VSInput_Particle
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

cbuffer ExternalData : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}

VToP_Particle main( VSInput_Particle input )
{
    //basic wvp matrix and calculate screen position
    VToP_Particle output;
    output.uv = input.uv;
    output.color = input.color;
    
    matrix worldViewProjectionMatrix = mul(projectionMatrix, mul(viewMatrix, worldMatrix));
    output.screenPosition = mul(worldViewProjectionMatrix, float4(input.position, 1.0f));
    
	return output;
}