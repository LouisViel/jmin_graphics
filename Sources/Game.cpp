//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "PerlinNoise.hpp"
#include "Engine/Shader.h"
#include "Engine/Buffers.h"
#include "Engine/VertexLayout.h"
#include "Engine/Texture.h"
#include "Engine/DefaultResources.h"
#include "Minicraft/World.h"
#include "Minicraft/Player.h"
#include "Minicraft/Utils.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

// Global stuff
DefaultResources gpuResources;
Shader basicShader(L"Basic");
Shader blockShader(L"Block");
VertexBuffer<VertexLayout_PositionColor> crosshairLine;

Texture texture(L"terrain");
World world;
Player player(&world, Vector3(16, 32, 16));
OrthographicCamera hudCamera(400, 600);

// Game
Game::Game() noexcept(false) {
	m_deviceResources = std::make_unique<DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, DXGI_FORMAT_D32_FLOAT, 2);
	m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game() {
	g_inputLayouts.clear();
}

void Game::Initialize(HWND window, int width, int height) {
	// Create input devices
	m_gamePad = std::make_unique<GamePad>();
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);
	m_mouse->SetMode(Mouse::MODE_RELATIVE);

	// Initialize the Direct3D resources
	m_deviceResources->SetWindow(window, width, height);
	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();

	basicShader.Create(m_deviceResources.get());
	blockShader.Create(m_deviceResources.get());
	GenerateInputLayout<VertexLayout_PositionColor>(m_deviceResources.get(), &basicShader);
	GenerateInputLayout<VertexLayout_PositionNormalUV>(m_deviceResources.get(), &blockShader);

	texture.Create(m_deviceResources.get());

	gpuResources.Create(m_deviceResources.get());

	player.GenerateGPUResources(m_deviceResources.get());
	player.GetCamera()->UpdateAspectRatio((float)width / (float)height);
	hudCamera.UpdateSize((float)width, (float)height);

	world.Generate(m_deviceResources.get());

	Vector3 dir(-0.5, -0.8, -0.2);
	dir.Normalize();
	auto res = Raycast(Vector3(20, 15, 20) + Vector3(0.5, 0.5, 0.5), dir, 20);

	for (auto& cube : res) {
		world.UpdateBlock(cube[0], cube[1], cube[2], STONE);
	}

	crosshairLine.PushVertex({ {-7, 0, 1, 1}, {1, 1, 1, 1} });
	crosshairLine.PushVertex({ {6, 0, 1, 1}, {1, 1, 1, 1} });
	crosshairLine.PushVertex({ {0, -6, 1, 1}, {1, 1, 1, 1} });
	crosshairLine.PushVertex({ {0, 7, 1, 1}, {1, 1, 1, 1} });

	crosshairLine.Create(m_deviceResources.get());
}

void Game::Tick() {
	// DX::StepTimer will compute the elapsed time and call Update() for us
	// We pass Update as a callback to Tick() because StepTimer can be set to a "fixed time" step mode, allowing us to call Update multiple time in a row if the framerate is too low (useful for physics stuffs)
	m_timer.Tick([&]() { Update(m_timer); });

	Render(m_timer);
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer) {
	auto const kb = m_keyboard->GetState();
	auto const ms = m_mouse->GetState();
	m_mouse->ResetScrollWheelValue();
	
	player.Update(timer.GetElapsedSeconds(), kb, ms);

	if (kb.Escape)
		ExitGame();

	auto const pad = m_gamePad->GetState(0);
}

// Draws the scene.
void Game::Render(DX::StepTimer const& timer) {
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
	
	ApplyInputLayout<VertexLayout_PositionNormalUV>(m_deviceResources.get());

	player.GetCamera()->ApplyCamera(m_deviceResources.get());

	blockShader.Apply(m_deviceResources.get());
	texture.Apply(m_deviceResources.get());
	world.Draw(player.GetCamera(), m_deviceResources.get());
	player.Draw(m_deviceResources.get());

	hudCamera.ApplyCamera(m_deviceResources.get());
	ApplyInputLayout<VertexLayout_PositionColor>(m_deviceResources.get());
	basicShader.Apply(m_deviceResources.get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	crosshairLine.Apply(m_deviceResources.get());
	context->Draw(4, 0);

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
	player.GetCamera()->UpdateAspectRatio((float)width / (float)height);
	hudCamera.UpdateSize((float)width, (float)height);
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
