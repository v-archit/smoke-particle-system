#include"ShaderIncludes.hlsli"

Texture2D AlbedoMap : register(t0); 
Texture2D NormalMap : register(t1);
Texture2D RoughnessMap : register(t2);
Texture2D MetalnessMap : register(t3);

SamplerState BasicSampler : register(s0);

//Constant buffer for pixel shader
cbuffer ExternalData : register(b0)
{
	float4 colorTint;  //surface color
    float3 cameraPosition;
    float roughness;
    float uvScale;
    float2 uvOffset;
    float applyNormalMap;
    Light lights[5];
}


// Lambert diffuse BRDF - Same as the basic lighting diffuse calculation!
// - NOTE: this function assumes the vectors are already NORMALIZED!
float DiffusePBR(float3 normal, float3 dirToLight)
{
    return saturate(dot(normal, dirToLight));
}

// Calculates diffuse amount based on energy conservation
//
// diffuse - Diffuse amount
// specular - Specular color (including light color)
// metalness - surface metalness amount
//
// Metals should have an albedo of (0,0,0)...mostly

float3 DiffuseEnergyConserve(float3 diffuse, float3 specular, float metalness)
{
    return diffuse * ((1 - saturate(specular)) * (1 - metalness));
}

// GGX (Trowbridge-Reitz)
//
// a - Roughness
// h - Half vector
// n - Normal
//
// D(h, n) = a^2 / pi * ((n dot h)^2 * (a^2 - 1) + 1)^2
float SpecDistribution(float3 n, float3 h, float roughness)
{
// Pre-calculations
    float NdotH = saturate(dot(n, h));
    float NdotH2 = NdotH * NdotH;
    float a = roughness * roughness;
    float a2 = max(a * a, MIN_ROUGHNESS); // Applied after remap!
// ((n dot h)^2 * (a^2 - 1) + 1)
    float denomToSquare = NdotH2 * (a2 - 1) + 1;
// Can go to zero if roughness is 0 and NdotH is 1; MIN_ROUGHNESS helps here
// Final value
    return a2 / (PI * denomToSquare * denomToSquare);
}

// Fresnel term - Schlick approx.
//
// v - View vector
// h - Half vector
// f0 - Value when l = n (full specular color)
//
// F(v,h,f0) = f0 + (1-f0)(1 - (v dot h))^5
float3 Fresnel(float3 v, float3 h, float3 f0)
{
// Pre-calculations
    float VdotH = saturate(dot(v, h));
// Final value
    return f0 + (1 - f0) * pow(1 - VdotH, 5);
}

// Geometric Shadowing - Schlick-GGX (based on Schlick-Beckmann)
// - k is remapped to a / 2, roughness remapped to (r+1)/2
//
// n - Normal
// v - View vector
//
// G(l,v)
float GeometricShadowing(float3 n, float3 v, float roughness)
{
// End result of remapping:
    float k = pow(roughness + 1, 2) / 8.0f;
    float NdotV = saturate(dot(n, v));
// Final value
    return NdotV / (NdotV * (1 - k) + k);
}


// Microfacet BRDF (Specular)
//
// f(l,v) = D(h)F(v,h)G(l,v,h) / 4(n dot l)(n dot v)
// - part of the denominator are canceled out by numerator (see below)
//
// D() - Spec Dist - Trowbridge-Reitz (GGX)
// F() - Fresnel - Schlick approx
// G() - Geometric Shadowing - Schlick-GGX
float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 specColor)
{
// Other vectors
    float3 h = normalize(v + l);
// Grab various functions
    float D = SpecDistribution(n, h, roughness);
    float3 F = Fresnel(v, h, specColor);
    float G = GeometricShadowing(n, v, roughness) * GeometricShadowing(n, l, roughness);
// Final formula
// Denominator dot products partially canceled by G()!

    return (D * F * G) / (4 * max(dot(n, v), dot(n, l)));
}

//Attenuate function for point light
float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.range * light.range)));
    return att * att;
}

//Calculate total light value for each light
float3 CalculateLightValues(VertexToPixel_NormalMap input, float3 viewVector, Light light, float3 albedoColor, float roughness, float metalness, float3 specularColor)
{
    float attenuateValue;
    float3 dirToLight, reflectionVector;
    
    
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
   
    
    //reflection vector of the light bouncing from surface
    reflectionVector = reflect(light.direction, input.normal);
    
    
    // Calculate the light amounts
    float diff = DiffusePBR(input.normal, dirToLight);
    float3 spec = MicrofacetBRDF(input.normal, dirToLight, viewVector, roughness, specularColor);
    
    // Calculate diffuse with energy conservation
    // (Reflected light doesn't get diffused)
    float3 balancedDiff = DiffuseEnergyConserve(diff, spec, metalness);
    
    // Combine the final diffuse and specular values for this light
    float3 total = (balancedDiff * albedoColor + spec) * light.intensity * light.color;
    
    
    return total * colorTint.xyz * attenuateValue;
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
float4 main(VertexToPixel_NormalMap input) : SV_TARGET
{
    float3 totalLights, viewVector, albedoColor, specularColor, unpackedNormal, orthogonalTangent, biTangent;
    float roughness, metalness;
    float3x3 TBN;
    
    //normals become unnormalized when interpolated
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
    input.uv = (uvScale * input.uv) + uvOffset;
    
    if(applyNormalMap)
    {
    //convert 0-1 rgb range[texture] to -1 to 1 range 
        unpackedNormal = NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1;
        
        orthogonalTangent = normalize(input.tangent - input.normal * dot(input.tangent, input.normal));
        biTangent = cross(orthogonalTangent, input.normal);
    //rotation matrix to transform normals from normal map
        TBN = float3x3(orthogonalTangent, biTangent, input.normal);
        input.normal = mul(unpackedNormal, TBN);
    }
    //un-corrected surface textures
    albedoColor = pow(AlbedoMap.Sample(BasicSampler, input.uv).rgb, 2.2f);
 
    roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
    
    metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;
    
    // Specular color determination -----------------
    // Assume albedo texture is actually holding specular color where metalness == 1
    //
    // Note the use of lerp here - metal is generally 0 or 1, but might be in between
    // because of linear texture sampling, so we lerp the specular color to match
    specularColor = lerp(F0_NON_METAL.rrr, albedoColor.rgb, metalness);
    
    //vector from pixel world position to camera position
    viewVector = normalize(cameraPosition - input.worldPosition);
    
    for (int i = 0; i < 5; i++)
    {
        totalLights += CalculateLightValues(input, viewVector, lights[i], albedoColor, roughness, metalness, specularColor);
    }
    
    return float4(pow(totalLights, 1.0f / 2.2f), 1);
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