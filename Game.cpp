#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "Helpers.h"

#include"ImGui/imgui.h"
#include"ImGui/imgui_impl_dx11.h"
#include"ImGui/imgui_impl_win32.h"


// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// Direct3D itself, and our window, are not ready at this point!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,			// The application's handle
		L"DirectX Game",	// Text for the window's title bar (as a wide-character string)
		1280,				// Width of the window's client area
		720,				// Height of the window's client area
		false,				// Sync the framerate to the monitor refresh? (lock framerate)
		true)				// Show extra stats (fps) in title bar?
{

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif

	//Initialize lights
	lightArray = {};

}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Delete all objects manually created within this class
//  - Release() all Direct3D objects created within this class
// --------------------------------------------------------
Game::~Game()
{
	// Call delete or delete[] on any objects or arrays you've
	// created using new or new[] within this class
	// - Note: this is unnecessary if using smart pointers

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// Call Release() on any Direct3D objects made within this class
	// - Note: this is unnecessary for D3D objects stored in ComPtrs
}

// --------------------------------------------------------
// Called once per program, after Direct3D and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();

	LoadTextures();
	CreateSamplerState();

	CreateMaterials();
	CreateGeometry();
	CreateEntities();

	CreateParticleStatesAndEmitters();

	CreateSkyBox();
	CreateLights();

	// Tell the input assembler (IA) stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our vertices?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//camera creation
	camera = std::make_shared<Camera>((float)this->windowWidth / this->windowHeight, DirectX::XMFLOAT3(0.0f, 1.0f, -2.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XM_PI / 3, 0.01f, 100.0f, 1.0f, 1.0f, true);

	//imgui stuff
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(device.Get(), context.Get());

	ImGui::StyleColorsDark();
}


void Game::CreateLights()
{
	//X (right) -> red directional

	Light light;

	light.type = LIGHT_TYPE_DIRECTIONAL;
	light.direction = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	light.intensity = 0.15f;
	light.color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

	lightArray.push_back(light);
	light = {};

	//-Y (down) -> green directional
	light.type = LIGHT_TYPE_DIRECTIONAL;
	light.direction = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
	light.intensity = 0.31f;
	light.color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

	lightArray.push_back(light);
	light = {};

	//Z (forward) -> blue directional
	light.type = LIGHT_TYPE_DIRECTIONAL;
	light.direction = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	light.intensity = 0.96f;
	light.color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

	lightArray.push_back(light);
	light = {};

	light.type = LIGHT_TYPE_POINT;
	light.position = DirectX::XMFLOAT3(0.12f, 1.82f, -3.5f);
	light.intensity = 1.0f;
	light.color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	light.range = 3.9f;

	lightArray.push_back(light);
	light = {};

	light.type = LIGHT_TYPE_POINT;
	light.position = DirectX::XMFLOAT3(0.0f, 0.0f, 5.0f);
	light.intensity = 1.17f;
	light.color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	light.range = 5.0f;

	lightArray.push_back(light);
	light = {};
}


// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// --------------------------------------------------------
void Game::LoadShaders()
{

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	
	vertexShader = std::make_shared<SimpleVertexShader>(device, context,
		FixPath(L"VertexShader.cso").c_str());

	pixelShader = std::make_shared<SimplePixelShader>(device, context,
		FixPath(L"PixelShader.cso").c_str());

	vertexShader_Normal = std::make_shared<SimpleVertexShader>(device, context,
		FixPath(L"VertexShader_Normal.cso").c_str());

	pixelShader_Normal = std::make_shared<SimplePixelShader>(device, context,
		FixPath(L"PixelShader_Normal.cso").c_str());

	vertexShader_Particles = std::make_shared<SimpleVertexShader>(device, context,
		FixPath(L"VertexShader_Particles.cso").c_str());

	pixelShader_Particles = std::make_shared<SimplePixelShader>(device, context,
		FixPath(L"PixelShader_Particles.cso").c_str());

	/*pixelShaderCustom = std::make_shared<SimplePixelShader>(device, context,
		FixPath(L"CustomPixelShader.cso").c_str());*/
		
}

void Game::LoadTextures()
{
	HRESULT isOk;
	
	//wood
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/wood_albedo.png").c_str(), nullptr, shaderViewWood.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/wood_normals.png").c_str(), nullptr, shaderViewWoodNormal.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/wood_roughness.png").c_str(), nullptr, shaderViewWoodRough.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/wood_metal.png").c_str(), nullptr, shaderViewWoodMetal.GetAddressOf());
	
	//stand
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/albedo_1.jpg").c_str(), nullptr, shaderViewStand.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/normal_1.png").c_str(), nullptr, shaderViewStandNormal.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/combo_1.png").c_str(),  nullptr, shaderViewStandRough.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/combo_1.png").c_str(),  nullptr, shaderViewStandMetal.GetAddressOf());

	//frame
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/albedo_2.jpg").c_str(), nullptr, shaderViewFrame.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/normal_2.png").c_str(), nullptr, shaderViewFrameNormal.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/combo_2.png").c_str(),  nullptr, shaderViewFrameRough.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/combo_2.png").c_str(),  nullptr, shaderViewFrameMetal.GetAddressOf());

	//chimney
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/albedo_3.jpg").c_str(), nullptr, shaderViewChimney.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/normal_3.png").c_str(), nullptr, shaderViewChimneyNormal.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/combo_3.png").c_str(),  nullptr, shaderViewChimneyRough.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/combo_3.png").c_str(),  nullptr, shaderViewChimneyMetal.GetAddressOf());

	//roof
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/albedo_4.jpg").c_str(), nullptr, shaderViewRoof.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/normal_4.png").c_str(), nullptr, shaderViewRoofNormal.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/combo_4.png").c_str(),  nullptr, shaderViewRoofRough.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/combo_4.png").c_str(),  nullptr, shaderViewRoofMetal.GetAddressOf());
	
	//door
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/albedo_5.jpg").c_str(), nullptr, shaderViewDoor.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/normal_5.png").c_str(), nullptr, shaderViewDoorNormal.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/combo_5.png").c_str(),  nullptr, shaderViewDoorRough.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/combo_5.png").c_str(),  nullptr, shaderViewDoorMetal.GetAddressOf());
	
	//walls
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/albedo_6.jpg").c_str(), nullptr, shaderViewWalls.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/normal_6.png").c_str(), nullptr, shaderViewWallsNormal.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/combo_6.png").c_str(),  nullptr, shaderViewWallsRough.GetAddressOf());
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/combo_6.png").c_str(),  nullptr, shaderViewWallsMetal.GetAddressOf());

	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/specular_map_default.png").c_str(), nullptr, shaderViewSpecMapDefault.GetAddressOf());
	
	//particle image
	isOk = DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/smoke_01.png").c_str(), nullptr, shaderViewParticle.GetAddressOf());
	if (isOk != S_OK)
		printf("Texture not loading");

}

void Game::CreateSamplerState()
{
	HRESULT isOk;

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 8;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	isOk = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
	if (isOk != S_OK)
		printf("Sampler state not created");

}

void Game::CreateMaterials()
{
	
	materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vertexShader_Normal, pixelShader_Normal));
	
	//house materials
	materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vertexShader_Normal, pixelShader_Normal));
	materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vertexShader_Normal, pixelShader_Normal));
	materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vertexShader_Normal, pixelShader_Normal));
	materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vertexShader_Normal, pixelShader_Normal));
	materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vertexShader_Normal, pixelShader_Normal));
	materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vertexShader_Normal, pixelShader_Normal));
	
	//particle material
	materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vertexShader_Particles, pixelShader_Particles));

	//Add texture SRV and sampler state to the material

	materials[0]->AddShaderView("AlbedoMap", shaderViewWood);
	materials[0]->AddShaderView("NormalMap", shaderViewWoodNormal);
	materials[0]->AddShaderView("RoughnessMap", shaderViewWoodRough);
	materials[0]->AddShaderView("MetalnessMap", shaderViewWoodMetal);
	materials[0]->AddSamplerState("BasicSampler", samplerState);

	materials[1]->AddShaderView("AlbedoMap", shaderViewStand);
	materials[1]->AddShaderView("NormalMap", shaderViewStandNormal);
	materials[1]->AddShaderView("RoughnessMap", shaderViewStandRough);
	materials[1]->AddShaderView("MetalnessMap", shaderViewStandMetal);
	materials[1]->AddSamplerState("BasicSampler", samplerState);

	materials[2]->AddShaderView("AlbedoMap",    shaderViewFrame);
	materials[2]->AddShaderView("NormalMap",    shaderViewFrameNormal);
	materials[2]->AddShaderView("RoughnessMap", shaderViewFrameRough);
	materials[2]->AddShaderView("MetalnessMap", shaderViewFrameMetal);
	materials[2]->AddSamplerState("BasicSampler", samplerState);

	materials[3]->AddShaderView("AlbedoMap",    shaderViewChimney);
	materials[3]->AddShaderView("NormalMap",    shaderViewChimneyNormal);
	materials[3]->AddShaderView("RoughnessMap", shaderViewChimneyRough);
	materials[3]->AddShaderView("MetalnessMap", shaderViewChimneyMetal);
	materials[3]->AddSamplerState("BasicSampler", samplerState);

	materials[4]->AddShaderView("AlbedoMap",    shaderViewRoof);
	materials[4]->AddShaderView("NormalMap",    shaderViewRoofNormal);
	materials[4]->AddShaderView("RoughnessMap", shaderViewRoofRough);
	materials[4]->AddShaderView("MetalnessMap", shaderViewRoofMetal);
	materials[4]->AddSamplerState("BasicSampler", samplerState);

	materials[5]->AddShaderView("AlbedoMap",    shaderViewDoor);
	materials[5]->AddShaderView("NormalMap",    shaderViewDoorNormal);
	materials[5]->AddShaderView("RoughnessMap", shaderViewDoorRough);
	materials[5]->AddShaderView("MetalnessMap", shaderViewDoorMetal);
	materials[5]->AddSamplerState("BasicSampler", samplerState);

	materials[6]->AddShaderView("AlbedoMap",    shaderViewWalls);
	materials[6]->AddShaderView("NormalMap",    shaderViewWallsNormal);
	materials[6]->AddShaderView("RoughnessMap", shaderViewWallsRough);
	materials[6]->AddShaderView("MetalnessMap", shaderViewWallsMetal);
	materials[6]->AddSamplerState("BasicSampler", samplerState);

	materials[7]->AddShaderView("ParticleMap", shaderViewParticle);
	materials[7]->AddSamplerState("BasicSampler", samplerState);
}

void Game::CreateSkyBox()
{
	std::vector<std::wstring> textureLocations;
	
	textureLocations.push_back(L"../../Assets/Textures/CubeMapTextures/right.png");  //right
	textureLocations.push_back(L"../../Assets/Textures/CubeMapTextures/left.png");  //left
	textureLocations.push_back(L"../../Assets/Textures/CubeMapTextures/up.png");  //up
	textureLocations.push_back(L"../../Assets/Textures/CubeMapTextures/down.png");  //down
	textureLocations.push_back(L"../../Assets/Textures/CubeMapTextures/front.png");  //front
	textureLocations.push_back(L"../../Assets/Textures/CubeMapTextures/back.png");  //back

	skyObject1 = std::make_shared<Sky>(textureLocations, cube, samplerState, device, context);
	//skyObject2 = std::make_shared<Sky>(FixPath(L"../../Assets/Textures/SunnyCubeMap.dds").c_str(), cube, samplerState, device, context);

}

// --------------------------------------------------------
void Game::CreateGeometry()
{
	//Create model objects
	cube = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/cube.obj").c_str(), device, context);
	helix = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/helix.obj").c_str(), device, context);
	stand = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/stand_1.obj").c_str(), device, context);
	frame = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/frame_2.obj").c_str(), device, context);
	chimney = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/chimney_3.obj").c_str(), device, context);
	roof = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/roof_4.obj").c_str(), device, context);
	door = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/door_5.obj").c_str(), device, context);
	walls = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/walls_6.obj").c_str(), device, context);
}

void Game::CreateEntities()
{
	//gameEntities.push_back(std::make_shared<GameEntity>(helix, materials[0]));
	gameEntities.push_back(std::make_shared<GameEntity>(stand, materials[1]));
	gameEntities.push_back(std::make_shared<GameEntity>(frame, materials[2]));
	gameEntities.push_back(std::make_shared<GameEntity>(chimney, materials[3]));
	gameEntities.push_back(std::make_shared<GameEntity>(roof, materials[4]));
	gameEntities.push_back(std::make_shared<GameEntity>(door, materials[5]));
	gameEntities.push_back(std::make_shared<GameEntity>(walls, materials[6]));

	//gameEntities[0]->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < gameEntities.size(); i++)
	{
		gameEntities[i]->GetTransform()->SetPosition(0.0f, 0.0f, 4.0f);
		gameEntities[i]->GetTransform()->SetScale(0.2f, 0.2f, 0.2f);
	}

}

void Game::CreateParticleStatesAndEmitters()
{
	//blend state
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	device->CreateBlendState(&blendDesc, blendState.GetAddressOf());

	//depth state
	D3D11_DEPTH_STENCIL_DESC depthDesc = {};
	depthDesc.DepthEnable = true;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;

	device->CreateDepthStencilState(&depthDesc, depthState.GetAddressOf());

	//smoke emitter
	smokeEmitter = std::make_shared<ParticleEmitter>(
		DirectX::XMFLOAT3(1.2f, 1.1f, 1.9f),             //position
		DirectX::XMFLOAT3(0.02f, 0.2f, 0.1f),             //start velocity
		materials[7],
		1000,                                            //max particles
		10,                                               //lifetime                          
		0.5f,                                              //emission time
		0.05f,                                            //start size
		1.0f,                                            //end size
		DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),	        // start color
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f),		    // end color
		device
	);

	smokeEmitter->InititalizeGeometry();

}


void Game::ImGuiUpdate(float deltaTime)
{
	// imgui stuff
		// Get a reference to our custom input manager
		Input & input = Input::GetInstance();
	// Reset input manager's gui state so we don’t
	// taint our own input (you’ll uncomment later)
	input.SetKeyboardCapture(false);
	input.SetMouseCapture(false);
	// Feed fresh input data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)this->windowWidth;
	io.DisplaySize.y = (float)this->windowHeight;
	io.KeyCtrl = input.KeyDown(VK_CONTROL);
	io.KeyShift = input.KeyDown(VK_SHIFT);
	io.KeyAlt = input.KeyDown(VK_MENU);
	io.MousePos.x = (float)input.GetMouseX();
	io.MousePos.y = (float)input.GetMouseY();
	io.MouseDown[0] = input.MouseLeftDown();
	io.MouseDown[1] = input.MouseRightDown();
	io.MouseDown[2] = input.MouseMiddleDown();
	io.MouseWheel = input.GetMouseWheel();
	input.GetKeyArray(io.KeysDown, 256);
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture (you’ll uncomment later)
	input.SetKeyboardCapture(io.WantCaptureKeyboard);
	input.SetMouseCapture(io.WantCaptureMouse);
	// Show the demo window
	//ImGui::ShowDemoWindow();
}

void Game::SelectLight()
{
	ImGui::Begin("Select light to change");

	if (ImGui::Button("Select Light.."))
		ImGui::OpenPopup("my_select_popup");
	ImGui::SameLine();
	ImGui::TextUnformatted(selected_light == -1 ? "<None>" : lightNames[selected_light]);
	if (ImGui::BeginPopup("my_select_popup"))
	{
		ImGui::Text("Lights");
		ImGui::Separator();
		for (int i = 0; i < IM_ARRAYSIZE(lightNames); i++)
			if (ImGui::Selectable(lightNames[i]))
				selected_light = i;
		ImGui::EndPopup();
	}

	ImGui::End();
}


void Game::ChangeLight()
{
	ImGui::Begin("Change light properties");

	if (selected_light < 3 && selected_light > -1)
	{
		ImGui::SliderFloat3("Direction X Y Z", &lightArray[selected_light].direction.x, -1.0f, 1.0f);
		ImGui::SliderFloat("Intensity", &lightArray[selected_light].intensity, 0.0f, 5.0f);
		ImGui::SliderFloat3("Color R G B", &lightArray[selected_light].color.x, 0.0f, 1.0f);
	}
	else if (selected_light > 2)
	{
		ImGui::SliderFloat3("Position X Y Z", &lightArray[selected_light].position.x, -5.0f, 5.0f);
		ImGui::SliderFloat("Range", &lightArray[selected_light].range, 0.0f, 10.0f);
		ImGui::SliderFloat("Intensity", &lightArray[selected_light].intensity, 0.0f, 5.0f);
		ImGui::SliderFloat3("Color R G B", &lightArray[selected_light].color.x, 0.0f, 1.0f);
	}
	else
		ImGui::Text("Select a light");


	ImGui::End();
}

void Game::ChangeTextureUV()
{
	std::shared_ptr<Material> material = gameEntities[3]->GetMaterial();

	ImGui::Begin("Change Cylinder UV");

	ImGui::SliderFloat("UV Scale", &material->uvScale, 0.0f, 10.0f);
	ImGui::SliderFloat2("UV Offset X Y", &material->uvOffset.x, -10.0f, 10.0f);

	ImGui::End();
}

void Game::ApplyMaps()
{

	ImGui::Begin("Apply Texture Maps");

	ImGui::Checkbox("Apply Normal Map", &materials[0]->applyNormalMap);

	ImGui::End();

	materials[1]->applyNormalMap = materials[0]->applyNormalMap;
	materials[2]->applyNormalMap = materials[0]->applyNormalMap;
}

void Game::DisplayFPS()
{
	ImGui::Begin("Display FPS");

	ImGuiIO& io = ImGui::GetIO();
	ImGui::Text("FPS: %f", io.Framerate);
	ImGui::Text("Mouse Position: X-> %f : Y-> %f", io.MousePos.x, io.MousePos.y);

	ImGui::End();
}

void Game::RotateObjectUI()
{
	ImGui::Begin("Rotate object");

	DirectX::XMFLOAT3 rotationObject = gameEntities[4]->GetTransform()->GetRotation();

	if (ImGui::SliderFloat("Helix Rotation", &rotationObject.y, -1.0f, 1.0f))
	{
		gameEntities[4]->GetTransform()->SetRotation(rotationObject.x, rotationObject.y, rotationObject.z);
	}

	ImGui::End();
}

void Game::DrawParticles()
{
	context->OMSetBlendState(blendState.Get(), 0, 0xffffffff);
	context->OMSetDepthStencilState(depthState.Get(), 0);

	smokeEmitter->DrawParticles(context, camera);

	//reset states
	context->OMSetBlendState(0, 0, 0xffffffff);
	context->OMSetDepthStencilState(0, 0);
}


// --------------------------------------------------------
// Handle resizing to match the new window size.
//  - DXCore needs to resize the back buffer
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	//update projection matrix
	if (camera != 0)
	{
		camera->UpdateProjectionMatrix((float)this->windowWidth / this->windowHeight);
	}
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	

	//update imgui
	ImGuiUpdate(deltaTime);
	
	//draw imgui windows

	//window to select light to change
	SelectLight();

	//window to change light properties
	ChangeLight();

	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();

	//update camera
	camera->Update(deltaTime);


	//simulate particles
	smokeEmitter->SimulateParticles(deltaTime, camera);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{

	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erases what's on the screen)
		const float bgColor[4] = { 0.4f, 0.6f, 0.75f, 1.0f }; // Cornflower Blue
		context->ClearRenderTargetView(backBufferRTV.Get(), bgColor);

		// Clear the depth buffer (resets per-pixel occlusion information)
		context->ClearDepthStencilView(depthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	
	//Draw entities

	for (int i = 0; i < gameEntities.size(); i++)
	{
		std::shared_ptr<Material> material = gameEntities[i]->GetMaterial();
		std::shared_ptr<SimpleVertexShader> vs = material->GetVertexShader();
		std::shared_ptr<SimplePixelShader> ps = material->GetPixelShader();

		ps->SetData("lights", &lightArray[0], sizeof(Light) * (int)lightArray.size());

		gameEntities[i]->Draw(context, camera);
	}

	//draw sky with 6 textures
	skyObject1->Draw(context, camera);

	//draw particles
	DrawParticles();

	//draw imgui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present the back buffer to the user
		//  - Puts the results of what we've drawn onto the window
		//  - Without this, the user never sees anything
		swapChain->Present(vsync ? 1 : 0, 0);

		// Must re-bind buffers after presenting, as they become unbound
		context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthBufferDSV.Get());
	}
}
