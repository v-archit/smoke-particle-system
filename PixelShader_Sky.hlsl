#include"ShaderIncludes.hlsli"

TextureCube CubeMap : register(t0); 

SamplerState BasicSampler : register(s0);

////Constant buffer for pixel shader
//cbuffer ExternalData : register(b0)
//{
	
//}


// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel_Sky input) : SV_TARGET
{
    float3 surfaceColor;
    
    surfaceColor = CubeMap.Sample(BasicSampler, input.sampleDir).rgb;
    
    return float4(surfaceColor, 1);
}
