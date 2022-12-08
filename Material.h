#pragma once

#include<DirectXMath.h>
#include<memory>
#include"SimpleShader.h"
#include<unordered_map>
#include"Transformation.h"
#include"Camera.h"

class Material
{
public:

	Material(DirectX::XMFLOAT4 colorTint, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader);
	~Material();

	float uvScale;
	//Specular map toggle
	bool applySpecMap = true;
	bool applyNormalMap = true;
	DirectX::XMFLOAT2 uvOffset;

#pragma region Getters
	DirectX::XMFLOAT4 GetColorTint();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	std::shared_ptr<SimplePixelShader> GetPixelShader();
#pragma endregion

#pragma region Setters
	void SetColorTint(DirectX::XMFLOAT4 colorTint);
	void SetRoughness(float roughness);
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> vertexShader);
	void SetPixelShader(std::shared_ptr<SimplePixelShader> pixelShader);

	void AddShaderView(const char* name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderView);
	void AddSamplerState(const char* name, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState);

	void PrepareMaterial(Transformation* transform, std::shared_ptr<Camera> camera);
#pragma endregion

private:
	float roughness;
	DirectX::XMFLOAT4 colorTint;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> mapShaderView;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> mapSamplerState;
};