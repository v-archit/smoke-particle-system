#include"ShaderIncludes.hlsli"

//Constant buffer for pixel shader
cbuffer ExternalData : register(b0)
{
	float4 colorTint;
    float1 totalTime;
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
    if ((input.screenPosition.x < 690.0f * (totalTime % 2)) && (input.screenPosition.y < 410.0f * (totalTime % 2)) && (input.screenPosition.x > 590.0f * (totalTime % 2)) && (input.screenPosition.y > 310.0f * (totalTime % 2)))
        return float4(0.4f, 0.6f, 0.75f, 1.0f);
    if ((input.screenPosition.x < 690.0f * (totalTime % 2)) && (input.screenPosition.y < 410.0f * (2 - (totalTime % 2))) && (input.screenPosition.x > 590.0f * (totalTime % 2)) && (input.screenPosition.y > 310.0f * (2 - (totalTime % 2))))
        return float4(0.4f, 0.6f, 0.75f, 1.0f);
    if ((input.screenPosition.x < (690.0f + (600 * (totalTime % 1.0f)))) && (input.screenPosition.y < (410.0f + (150 * (totalTime % 1.0f)))) && (input.screenPosition.x > (590.0f - (300 * (totalTime % 1.0f)))) && (input.screenPosition.y > (310.0f - (150 * (totalTime % 1.0f)))))
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    else
    {
        //blinking effect
        return float4(input.uv * ((totalTime * 10 % 1.0f)), 0.5f, 1);
    }
}