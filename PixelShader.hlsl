#include"ShaderIncludes.hlsli"

Texture2D SurfaceTexture : register(t0); 
Texture2D SpecularMap : register(t1);
SamplerState BasicSampler : register(s0);

//Constant buffer for pixel shader
cbuffer ExternalData : register(b0)
{
	float4 colorTint;  //surface color
    float3 cameraPosition;
    float roughness;
    float3 ambientColor;
    float uvScale;
    float2 uvOffset;
    float applySpecMap;
    Light lights[5];
}


//Attenuate function for point light
float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.range * light.range)));
    return att * att;
}


float3 CalculateLightValues(VertexToPixel input, float3 viewVector, Light light, float3 surfaceColor, float specScale)
{
    float diffuseLight, specExponent, specularLight, attenuateValue;
    float3 dirToLight, reflectionVector, totalLightValues;
    
    
    if (light.type == LIGHT_TYPE_POINT)
    {
        light.direction = normalize(input.worldPosition - light.position);
        attenuateValue = Attenuate(light, input.worldPosition);
    }
    else
    {
        attenuateValue = 1.0f;
    }
    
    //vector to the light source
    dirToLight = normalize(-1 * light.direction);
    
    //Calculate diffuse light amount -> N Dot L
    //saturate-> between 0 and 1
	//dot product(v1,v2)
    diffuseLight = saturate(dot(input.normal, dirToLight));
    
    //reflection vector of the light bouncing from surface
    reflectionVector = reflect(light.direction, input.normal);
    
    specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
    
    //If roughness is 1, spec exponent will be 0 resulting into total white specular component [pow(x,0) -> 1]
    if (specExponent > 0.05)
        specularLight = pow(saturate(dot(reflectionVector, viewVector)), specExponent);
    else
        specularLight = 0;
    
    //Total light components calculation
    //apply spec map
    if (applySpecMap)
        totalLightValues = diffuseLight + (specularLight * specScale);
    else
        totalLightValues = diffuseLight + (specularLight);
    
    
    return totalLightValues * light.color * surfaceColor * colorTint.xyz * attenuateValue * light.intensity;
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
    
    float3 totalLights, viewVector, surfaceColor;
    float specMapScale;
    
    //normals become unnormalized when interpolated
    input.normal = normalize(input.normal);
    input.uv = (uvScale * input.uv) + uvOffset;
    
    surfaceColor = pow(SurfaceTexture.Sample(BasicSampler, input.uv).rgb, 2.2f);
 
    specMapScale = SpecularMap.Sample(BasicSampler, input.uv).r;
    
    //vector from pixel world position to camera position
    viewVector = normalize(cameraPosition - input.worldPosition);
    
    for (int i = 0; i < 5; i++)
    {
        totalLights += CalculateLightValues(input, viewVector, lights[i], surfaceColor, specMapScale);
    }
    
    //gamma corrected
    return float4(pow(totalLights + (ambientColor * colorTint.wxy * surfaceColor), 1.0f / 2.2f), 1);
}
 //   //vector to the light source
 //   dirToLight = normalize(-dirLight1.direction);
    
 //   //Calculate diffuse light amount -> N Dot L
 //   //saturate-> between 0 and 1
	////dot product(v1,v2)
 //   diffuseLight = saturate(dot(input.normal, dirToLight));
    
 //   //reflection vector of the light bouncing from surface
 //   reflectionVector = reflect(dirLight1.direction, input.normal);
    
 //   //If roughness is 1, spec exponent will be 0 resulting into total white specular component [pow(x,0) -> 1]
 //   if(specExponent > 0.05)
 //       specularLight = pow(saturate(dot(reflectionVector, viewVector)), specExponent);
 //   else
 //       specularLight = 0;
	
 //   //Total light components calculation
 //   totalLightValues = diffuseLight + specularLight;