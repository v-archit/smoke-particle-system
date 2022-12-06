#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <memory>
#include<vector>
#include"Mesh.h"
#include"GameEntity.h"
#include"Camera.h"
#include"SimpleShader.h"
#include"Material.h"
#include"Lights.h"
#include"WICTextureLoader.h"
#include"Sky.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();
	void LoadTextures();
	void CreateSamplerState();
	void CreateMaterials();
	void CreateSkyBox();

	void CreateGeometry();
	void CreateEntities();

	void CreateLights();

	//Imgui functions
	void ImGuiUpdate(float deltaTime);
	void SelectLight();
	void DisplayFPS();
	void ChangeLight();
	void ChangeTextureUV();
	void ApplyMaps();
	void RotateObjectUI();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	//// Buffers to hold actual geometry data
	//Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	//Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	// Shaders and shader-related constructs
		/*Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;*/

	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader_Normal;
	std::shared_ptr<SimplePixelShader> pixelShader_Normal;

	// Mesh Stuff

	std::shared_ptr<Mesh> triangle;
	std::shared_ptr<Mesh> square;
	std::shared_ptr<Mesh> pentagon;

	std::shared_ptr<Mesh> sphere;
	std::shared_ptr<Mesh> torus;
	std::shared_ptr<Mesh> cube;
	std::shared_ptr<Mesh> cylinder;
	std::shared_ptr<Mesh> helix;
	std::shared_ptr<Mesh> quad;
	std::shared_ptr<Mesh> quad_double_sided;

	// Entities stuff

	std::vector<std::shared_ptr<GameEntity>> gameEntities;

	// Materials stuff

	std::vector < std::shared_ptr<Material> > materials;

	// Camera stuff
	std::shared_ptr<Camera> camera;

	// Light stuff

	std::vector<Light> lightArray;

	// Texture stuff

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewCobble;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewCobbleNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewCobbleRough;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewCobbleMetal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewBronze;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewBronzeNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewBronzeRough;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewBronzeMetal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewWood;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewWoodNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewWoodRough;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewWoodMetal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewSpecMap;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewSpecMapDefault;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	int selected_light = -1;
	const char* lightNames[5] = { "Dir Light 1", "Dir Light 2", "Dir Light 3", "Point Light 1", "Point Light 2" };

	//Cubemap stuff

	std::shared_ptr<Sky> skyObject1;
	std::shared_ptr<Sky> skyObject2;
};

