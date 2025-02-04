//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "PerlinNoise.hpp"
#include "Engine/Shader.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

// Global stuff
Shader* basicShader;
ComPtr<ID3D11InputLayout> inputLayout;
ComPtr<ID3D11Buffer> vertexBuffer;
ComPtr<ID3D11Buffer> indexBuffer;
ComPtr<ID3D11Buffer> modelBuffer;
ComPtr<ID3D11Buffer> cameraBuffer;

// Camera Settings
Vector3 camPos;
Quaternion camRot;
Matrix mWorld;
Matrix mLookat;
Matrix mpfov;

// Matrix Data Structures to pass to GPU
struct ModelData {
	Matrix Model;
};

struct CameraData {
	Matrix View;
	Matrix Projection;
};

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

	auto device = m_deviceResources->GetD3DDevice();

	const std::vector<D3D11_INPUT_ELEMENT_DESC> InputElementDescs = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	device->CreateInputLayout(
		InputElementDescs.data(), InputElementDescs.size(),
		basicShader->vsBytecode.data(), basicShader->vsBytecode.size(),
		inputLayout.ReleaseAndGetAddressOf());

	// START CUSTOM CODE AREA

	// Setup Camera Datas
	camPos = Vector3::Zero;
	camRot = Quaternion::LookRotation(Vector3::Forward, Vector3::Up);
	mWorld = Matrix::CreateWorld(Vector3::Zero, Vector3::Forward, Vector3::Up);
	mLookat = Matrix::CreateFromQuaternion(camRot);
	mpfov = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(90.0f), (float)width/(float)height, 0.01f, 100.0f);


	// Alloue Vertex Buffer
	std::vector<float> vertexs = {
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f
	};
	CD3D11_BUFFER_DESC descVert(sizeof(float) * vertexs.size(), D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA subResDataVert = { };
	subResDataVert.pSysMem = vertexs.data(); // pointeur vers la data
	device->CreateBuffer(&descVert, &subResDataVert, vertexBuffer.ReleaseAndGetAddressOf());


	// Alloue Index Buffer
	std::vector<uint32_t> indexs = {
		0, 1, 2,
		2, 1, 3
	};
	CD3D11_BUFFER_DESC descInd(sizeof(uint32_t) * indexs.size(), D3D11_BIND_INDEX_BUFFER);
	D3D11_SUBRESOURCE_DATA subResDataInd = { };
	subResDataInd.pSysMem = indexs.data(); // pointeur vers la data
	device->CreateBuffer(&descInd, &subResDataInd, indexBuffer.ReleaseAndGetAddressOf());


	// Alloue Model + Camera Matrix Buffers
	CD3D11_BUFFER_DESC descModel(sizeof(ModelData), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&descModel, nullptr, modelBuffer.ReleaseAndGetAddressOf());
	CD3D11_BUFFER_DESC descCamera(sizeof(CameraData), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&descCamera, nullptr, cameraBuffer.ReleaseAndGetAddressOf());

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
	mLookat = Matrix::CreateLookAt(
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
	//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // CUSTOM CODE
	context->IASetInputLayout(inputLayout.Get());

	basicShader->Apply(m_deviceResources.get());




	// START CUSTOM CODE AREA

	// Inject/Set Camera & Model Buffers
	ID3D11Buffer* mcbs[] = { cameraBuffer.Get(), modelBuffer.Get() };
	context->VSSetConstantBuffers(0, 2, mcbs);

	// Update Camera Buffer 
	CameraData cameraData = { };
	cameraData.View = mLookat.Transpose();
	cameraData.Projection = mpfov.Transpose();
	context->UpdateSubresource(cameraBuffer.Get(), 0, nullptr, &cameraData, 0, 0);

	// Update Model Buffer
	ModelData modelData = { };
	//modelData.Model = mWorld.Transpose();
	modelData.Model = Matrix::CreateTranslation(Vector3(0.5f, 0, 0)).Transpose();
	context->UpdateSubresource(modelBuffer.Get(), 0, nullptr, &modelData, 0, 0);

	// Set Vertex Buffer
	ID3D11Buffer* vbs[] = { vertexBuffer.Get() };
	const unsigned int strides[] = { sizeof(float) * 3 };
	const unsigned int offsets[] = { 0 };
	context->IASetVertexBuffers(0, 1, vbs, strides, offsets);
	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Draw Indexed Render
	context->DrawIndexed(6, 0, 0);

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
