#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr and Buffers
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

	std::shared_ptr<Mesh> cube;
	std::shared_ptr<Mesh> helix;
	std::shared_ptr<Mesh> stand;
	std::shared_ptr<Mesh> frame;
	std::shared_ptr<Mesh> chimney;
	std::shared_ptr<Mesh> roof;
	std::shared_ptr<Mesh> door;
	std::shared_ptr<Mesh> walls;

	// Entities stuff

	std::vector<std::shared_ptr<GameEntity>> gameEntities;

	// Materials stuff

	std::vector < std::shared_ptr<Material> > materials;

	// Camera stuff
	std::shared_ptr<Camera> camera;

	// Light stuff

	std::vector<Light> lightArray;

	// Texture stuff

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewWood;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewWoodNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewWoodRough;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewWoodMetal;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewStand;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewStandNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewStandRough;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewStandMetal;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewFrame;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewFrameNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewFrameRough;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewFrameMetal;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewChimney;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewChimneyNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewChimneyRough;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewChimneyMetal;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewRoof;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewRoofNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewRoofRough;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewRoofMetal;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewDoor;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewDoorNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewDoorRough;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewDoorMetal;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewWalls;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewWallsNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewWallsRough;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewWallsMetal;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewSpecMap;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewSpecMapDefault;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	int selected_light = -1;
	const char* lightNames[5] = { "Dir Light 1", "Dir Light 2", "Dir Light 3", "Point Light 1", "Point Light 2" };

	//Cubemap stuff

	std::shared_ptr<Sky> skyObject1;
	std::shared_ptr<Sky> skyObject2;
};

