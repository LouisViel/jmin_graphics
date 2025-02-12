#pragma once

#include "Engine/DepthState.h"
#include "Engine/Camera.h"
#include "Minicraft/World.h"
#include "Minicraft/Cube3D.h"

using namespace DirectX::SimpleMath;

class Player {
	World* world = nullptr;

	Vector3 position = Vector3();
	float velocityY = 0;

	float walkSpeed = 10.0f;

	PerspectiveCamera camera = PerspectiveCamera(75, 1);

	Cube3D currentCube = Cube3D(HALF_SLAB);
	Cube3D highlightCube = Cube3D(HIGHLIGHT);

	DirectX::Mouse::ButtonStateTracker      mouseTracker;
	DirectX::Keyboard::KeyboardStateTracker keyboardTracker;
public:
	Player(World* w, Vector3 pos) : world(w), position(pos) {}

	void GenerateGPUResources(DeviceResources* deviceRes);
	void Update(float dt, DirectX::Keyboard::State kb, DirectX::Mouse::State ms);
	void Draw(DeviceResources* deviceRes);

	PerspectiveCamera* GetCamera() { return &camera; }
};
