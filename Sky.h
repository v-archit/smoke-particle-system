#pragma once

#include <wrl/client.h>
#include "DXCore.h"
#include <memory>
#include"Mesh.h"
#include"Camera.h"
#include"SimpleShader.h"
#include<vector>

class Sky
{
public:
	Sky(std::vector<std::wstring> textureLocations, std::shared_ptr<Mesh> cubeMesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState,
		Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	Sky(const wchar_t* ddsLocation, std::shared_ptr<Mesh> cubeMesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, 
		Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera);

private:

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewCube;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthBufferDSV;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	std::shared_ptr<Mesh> skyMesh;
	std::shared_ptr<SimplePixelShader> pixelShader_Sky;
	std::shared_ptr<SimpleVertexShader> vertexShader_Sky;

};