//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "PerlinNoise.hpp"
#include "Engine/Shader.h"

#include "Engine/VertexLayout.h";
#include "Engine/Buffers.h";
#include "Custom/MeshHelper.h";

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;
using VL_PUV = VertexLayout_PositionUV;

// Matrix Data Structures to pass to GPU
struct ModelData {
	Matrix Model;
};

struct CameraData {
	Matrix View;
	Matrix Projection;
};

// Global stuff
Shader* basicShader;
ComPtr<ID3D11InputLayout> inputLayout;
VertexBuffer<VL_PUV> vertexBuffer;
IndexBuffer indexBuffer;
ConstantBuffer<ModelData> modelBuffer;
ConstantBuffer<CameraData> cameraBuffer;

// Camera Settings
Vector3 camPos;
Quaternion camRot;
Matrix mWorld;
Matrix mView;
Matrix mProjection;

// Game
Game::Game() noexcept(false) {
	m_deviceResources = std::make_unique<DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_D32_FLOAT, 2);
	m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game() {
	delete basicShader;
	g_inputLayouts.clear();
}

void Game::Initialize(HWND window, int width, int height) {
	// Create input devices
	m_gamePad = std::make_unique<GamePad>();
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);

	// Initialize the Direct3D resources
	m_deviceResources->SetWindow(window, width, height);
	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();

	basicShader = new Shader(L"Basic");
	basicShader->Create(m_deviceResources.get());


	// START CUSTOM CODE AREA

	// Generate Input Layout
	ID3D11Device1* device = m_deviceResources->GetD3DDevice();
	GenerateInputLayout<VL_PUV>(m_deviceResources.get(), basicShader);

	// Setup Camera Datas
	camPos = Vector3::Zero;
	camRot = Quaternion::LookRotation(Vector3::Forward, Vector3::Up);
	mWorld = Matrix::CreateWorld(Vector3::Zero, Vector3::Forward, Vector3::Up);
	mView = Matrix::CreateFromQuaternion(camRot);
	mProjection = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(90.0f), (float)width/(float)height, 0.01f, 100.0f);


	/*std::vector<float> vertexs = {};
	std::vector<uint32_t> indexs = {};
	MeshHelper::PushCW(vertexs, indexs, Vector3(0, 0, 3.0f), 1.0f);*/


	// Alloue Vertex Buffer
	std::vector<VL_PUV> vertexs = {
		{{ -0.5f, 0.5f, 0.0f, 0.0f }, { 0.0f, 1.0f }},
		{{ 0.5f, 0.5f, 0.0f, 0.0f }, { 1.0f, 1.0f }},
		{{ -0.5f, -0.5f, 0.0f, 0.0f }, { 0.0f, 0.0f }},
		{{ 0.5f, -0.5f, 0.0f, 0.0f }, { 1.0f, 0.0f }},
	};

	std::vector<VL_PUV>* vbuf = vertexBuffer.get();
	vbuf->insert(vbuf->end(), vertexs.begin(), vertexs.end());
	vertexBuffer.Create(device);


	// Alloue Index Buffer
	std::vector<uint32_t> indexs = {
		0, 1, 2,
		2, 1, 3
	};

	std::vector<uint32_t>* ibuf = indexBuffer.get();
	ibuf->insert(ibuf->end(), indexs.begin(), indexs.end());
	indexBuffer.Create(device);


	// Alloue Model + Camera Matrix Buffers
	modelBuffer.Create(device);
	cameraBuffer.Create(device);

	// END OF CUSTOM CODE AREA
}

void Game::Tick() {
	// DX::StepTimer will compute the elapsed time and call Update() for us
	// We pass Update as a callback to Tick() because StepTimer can be set to a "fixed time" step mode, allowing us to call Update multiple time in a row if the framerate is too low (useful for physics stuffs)
	m_timer.Tick([&]() { Update(m_timer); });

	Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer) {
	auto const kb = m_keyboard->GetState();
	auto const ms = m_mouse->GetState();
	
	// START CUSTOM CODE AREA

	// add kb/mouse interact here
	mView = Matrix::CreateLookAt(
		//Vector3(0, 0, 2),
		Vector3(2 * sin(timer.GetTotalSeconds()), 0, 2 * cos(timer.GetTotalSeconds())),
		Vector3::Zero,
		Vector3::Up
	);

	// END CUSTOM CODE AREA
	
	if (kb.Escape)
		ExitGame();

	auto const pad = m_gamePad->GetState(0);
}

// Draws the scene.
void Game::Render() {
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
		return;

	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();
	auto const viewport = m_deviceResources->GetScreenViewport();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);
	
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(inputLayout.Get());

	ApplyInputLayout<VL_PUV>(m_deviceResources.get());

	basicShader->Apply(m_deviceResources.get());



	// START CUSTOM CODE AREA

	// Apply Buffers to Context
	vertexBuffer.Apply(context);
	indexBuffer.Apply(context);
	cameraBuffer.Apply(context, 0);
	modelBuffer.Apply(context, 1);

	// Update Camera Constant Buffer
	CameraData* cameraData = cameraBuffer.get();
	cameraData->View = mView.Transpose();
	cameraData->Projection = mProjection.Transpose();
	cameraBuffer.Update(context);

	// TODO : Update Context and Draw for each MODEL (face)
	//context->DrawIndexed(6 * 6, 0, 0); // En fait mon code marchait maybe, mais faut re-coder avec le nouveau système là

	// Temp/Debug Code
	ModelData* modelData = modelBuffer.get();
	modelData->Model = mWorld.Transpose();
	modelBuffer.Update(context);

	int a = indexBuffer.get()->size();
	context->DrawIndexed(indexBuffer.get()->size(), 0, 0);

	// END OF CUSTOM CODE AREA


	// envoie nos commandes au GPU pour etre afficher � l'�cran
	m_deviceResources->Present();
}


#pragma region Message Handlers
void Game::OnActivated() {}

void Game::OnDeactivated() {}

void Game::OnSuspending() {}

void Game::OnResuming() {
	m_timer.ResetElapsedTime();
}

void Game::OnWindowMoved() {
	auto const r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange() {
	m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height) {
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	// The windows size has changed:
	// We can realloc here any resources that depends on the target resolution (post processing etc)
}

void Game::OnDeviceLost() {
	// We have lost the graphics card, we should reset resources [TODO]
}

void Game::OnDeviceRestored() {
	// We have a new graphics card context, we should realloc resources [TODO]
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept {
	width = 800;
	height = 600;
}

#pragma endregion
