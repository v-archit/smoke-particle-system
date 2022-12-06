#include "Sky.h"
#include "Helpers.h"
#include"WICTextureLoader.h"
#include"DDSTextureLoader.h"
#include<iostream>

Sky::Sky(std::vector<std::wstring> textureLocations, std::shared_ptr<Mesh> cubeMesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	skyMesh = cubeMesh;

	vertexShader_Sky = std::make_shared<SimpleVertexShader>(device, context,
		FixPath(L"VertexShader_Sky.cso").c_str());

	pixelShader_Sky = std::make_shared<SimplePixelShader>(device, context,
		FixPath(L"PixelShader_Sky.cso").c_str());

	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_FRONT;

	device->CreateRasterizerState(&rasterDesc, rasterizerState.GetAddressOf());

	D3D11_DEPTH_STENCIL_DESC depthDesc = {};
	depthDesc.DepthEnable = true;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	device->CreateDepthStencilState(&depthDesc, depthBufferDSV.GetAddressOf());

	//Cubemap generation from 6 textures

	// Load the 6 textures into an array.
	// - We need references to the TEXTURES, not SHADER RESOURCE VIEWS!
	// - Explicitly NOT generating mipmaps, as we don't need them for the sky!
	// - Order matters here! +X, -X, +Y, -Y, +Z, -Z
	Microsoft::WRL::ComPtr<ID3D11Texture2D> textures[6] = {};
	DirectX::CreateWICTextureFromFile(device.Get(), FixPath(textureLocations[0]).c_str(), (ID3D11Resource**)textures[0].GetAddressOf(), 0);  //right
	DirectX::CreateWICTextureFromFile(device.Get(), FixPath(textureLocations[1]).c_str(), (ID3D11Resource**)textures[1].GetAddressOf(), 0);	//left
	DirectX::CreateWICTextureFromFile(device.Get(), FixPath(textureLocations[2]).c_str(), (ID3D11Resource**)textures[2].GetAddressOf(), 0);	//up
	DirectX::CreateWICTextureFromFile(device.Get(), FixPath(textureLocations[3]).c_str(), (ID3D11Resource**)textures[3].GetAddressOf(), 0);	//down
	DirectX::CreateWICTextureFromFile(device.Get(), FixPath(textureLocations[4]).c_str(), (ID3D11Resource**)textures[4].GetAddressOf(), 0);	//front
	DirectX::CreateWICTextureFromFile(device.Get(), FixPath(textureLocations[5]).c_str(), (ID3D11Resource**)textures[5].GetAddressOf(), 0);	//back
	// We'll assume all of the textures are the same color format and resolution,
	// so get the description of the first shader resource view
	D3D11_TEXTURE2D_DESC faceDesc = {};
	textures[0]->GetDesc(&faceDesc);
	// Describe the resource for the cube map, which is simply
	// a "texture 2d array" with the TEXTURECUBE flag set.
	// This is a special GPU resource format, NOT just a
	// C++ array of textures!!!
	D3D11_TEXTURE2D_DESC cubeDesc = {};
	cubeDesc.ArraySize = 6; // Cube map!
	cubeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // We'll be using as a texture in a shader
	cubeDesc.CPUAccessFlags = 0; // No read back
	cubeDesc.Format = faceDesc.Format; // Match the loaded texture's color format
	cubeDesc.Width = faceDesc.Width; // Match the size
	cubeDesc.Height = faceDesc.Height; // Match the size
	cubeDesc.MipLevels = 1; // Only need 1
	cubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE; // A CUBE, not 6 separate textures
	cubeDesc.Usage = D3D11_USAGE_DEFAULT; // Standard usage
	cubeDesc.SampleDesc.Count = 1;
	cubeDesc.SampleDesc.Quality = 0;
	// Create the final texture resource to hold the cube map
	Microsoft::WRL::ComPtr<ID3D11Texture2D> cubeMapTexture;
	device->CreateTexture2D(&cubeDesc, 0, cubeMapTexture.GetAddressOf());
	// Loop through the individual face textures and copy them,
	// one at a time, to the cube map texure
	for (int i = 0; i < 6; i++)
	{
		// Calculate the subresource position to copy into
		unsigned int subresource = D3D11CalcSubresource(
			0, // Which mip (zero, since there's only one)
			i, // Which array element?
			1); // How many mip levels are in the texture?
		// Copy from one resource (texture) to another
		context->CopySubresourceRegion(
			cubeMapTexture.Get(), // Destination resource
			subresource, // Dest subresource index (one of the array elements)
			0, 0, 0, // XYZ location of copy
			textures[i].Get(), // Source resource
			0, // Source subresource index (we're assuming there's only one)
			0); // Source subresource "box" of data to copy (zero means the whole thing)
	}
	// At this point, all of the faces have been copied into the
	// cube map texture, so we can describe a shader resource view for it
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = cubeDesc.Format; // Same format as texture
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; // Treat this as a cube!
	srvDesc.TextureCube.MipLevels = 1; // Only need access to 1 mip
	srvDesc.TextureCube.MostDetailedMip = 0; // Index of the first mip we want to see
	// Make the SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV;
	device->CreateShaderResourceView(cubeMapTexture.Get(), &srvDesc, cubeSRV.GetAddressOf());

	//local shader view copy
	shaderViewCube = cubeSRV;
}

Sky::Sky(const wchar_t* ddsLocation, std::shared_ptr<Mesh> cubeMesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	vertexShader_Sky = std::make_shared<SimpleVertexShader>(device, context,
		FixPath(L"VertexShader_Sky.cso").c_str());

	pixelShader_Sky = std::make_shared<SimplePixelShader>(device, context,
		FixPath(L"PixelShader_Sky.cso").c_str());

	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_FRONT;

	device->CreateRasterizerState(&rasterDesc, rasterizerState.GetAddressOf());

	D3D11_DEPTH_STENCIL_DESC depthDesc = {};
	depthDesc.DepthEnable = true;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	device->CreateDepthStencilState(&depthDesc, depthBufferDSV.GetAddressOf());

	//Cubemap generation from dds file

	HRESULT isOk;
	isOk = DirectX::CreateDDSTextureFromFile(device.Get(), ddsLocation, nullptr, shaderViewCube.GetAddressOf());
	if (isOk != S_OK)
		printf("DDS file not loading");
}

void Sky::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera)
{
	context->RSSetState(rasterizerState.Get());
	context->OMSetDepthStencilState(depthBufferDSV.Get(), 0);

	vertexShader_Sky->SetMatrix4x4("viewMatrix", camera->GetViewMatrix());
	vertexShader_Sky->SetMatrix4x4("projectionMatrix", camera->GetProjectionMatrix());

	pixelShader_Sky->SetShaderResourceView("CubeMap", shaderViewCube);
	pixelShader_Sky->SetSamplerState("BasicSampler", samplerState);

	vertexShader_Sky->SetShader();
	pixelShader_Sky->SetShader();

	vertexShader_Sky->CopyAllBufferData();
	pixelShader_Sky->CopyAllBufferData();

	skyMesh->Draw();

	context->RSSetState(nullptr);
	context->OMSetDepthStencilState(nullptr, 0);
}
