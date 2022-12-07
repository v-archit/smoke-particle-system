#include "Material.h"

Material::Material(DirectX::XMFLOAT4 colorTint, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader)
{
    this->colorTint = colorTint;
    this->vertexShader = vertexShader;
    this->pixelShader = pixelShader;
    this->uvScale = 1.0f;
    this->uvOffset = DirectX::XMFLOAT2(0.0f, 0.0f);
}

Material::~Material()
{
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
    return colorTint;
}

//float Material::GetRoughness()
//{
//    return roughness;
//}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
    return vertexShader;
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
    return pixelShader;
}

void Material::SetColorTint(DirectX::XMFLOAT4 colorTint)
{
    this->colorTint = colorTint;
}

void Material::SetRoughness(float roughness)
{
    this->roughness = roughness;
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> vertexShader)
{
    this->vertexShader = vertexShader;
}

void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> pixelShader)
{
    this->pixelShader = pixelShader;
}

void Material::AddShaderView(const char* name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderView)
{
    mapShaderView.insert({ name, shaderView });
}

void Material::AddSamplerState(const char* name, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState)
{
    mapSamplerState.insert({ name, samplerState });
}

void Material::PrepareMaterial()
{
    int i = 0;
    int j = 0;
    for (auto& t : mapShaderView) {
        pixelShader->SetShaderResourceView(t.first.c_str(), t.second);
    }
    for (auto& s : mapSamplerState) { 
        pixelShader->SetSamplerState(s.first.c_str(), s.second); 
    }

    //material specific properties via simple shader
    pixelShader->SetFloat4("colorTint", GetColorTint());
   // pixelShader->SetFloat("roughness", GetRoughness());
    pixelShader->SetFloat("uvScale", uvScale);
    pixelShader->SetFloat2("uvOffset", uvOffset);
    pixelShader->SetFloat("applySpecMap", static_cast<float>(applySpecMap));
    pixelShader->SetFloat("applyNormalMap", static_cast<float>(applyNormalMap));
}
