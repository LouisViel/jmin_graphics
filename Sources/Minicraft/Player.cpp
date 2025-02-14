#include "pch.h"

#include "Engine/DefaultResources.h"
#include "Player.h"
#include "Utils.h"

using ButtonState = Mouse::ButtonStateTracker::ButtonState;

Vector3 collisionPoints[] = {
	{ 0.3f,     0,     0},
	{-0.3f,     0,     0},
	{    0,     0,  0.3f},
	{    0,     0, -0.3f},
	{    0, -0.5f,     0},
	{ 0.3f,  1.0f,     0},
	{-0.3f,  1.0f,     0},
	{    0,  1.0f,  0.3f},
	{    0,  1.0f, -0.3f},
	{    0,  1.5f,     0},
};

void Player::GenerateGPUResources(DeviceResources* deviceRes) {
	currentCube.Generate(deviceRes);
	highlightCube.Generate(deviceRes);
}

void Player::Update(float dt, DirectX::Keyboard::State kb, DirectX::Mouse::State ms) {
	keyboardTracker.Update(kb);
	mouseTracker.Update(ms);

	Vector3 delta;
	if (kb.Z) delta += Vector3::Forward;
	if (kb.S) delta += Vector3::Backward;
	if (kb.Q) delta += Vector3::Left;
	if (kb.D) delta += Vector3::Right;
	Vector3 move = Vector3::TransformNormal(delta, camera.GetInverseViewMatrix());
	move.y = 0.0;
	move.Normalize();
	position += move * walkSpeed * dt;

	Quaternion camRot = camera.GetRotation();
	camRot *= Quaternion::CreateFromAxisAngle(camera.Right(), -ms.y * dt * 0.25f);
	camRot *= Quaternion::CreateFromAxisAngle(Vector3::Up, -ms.x * dt * 0.25f);
	
	velocityY += -30 * dt;

	Vector3 nextPos = position + Vector3(0, velocityY, 0) * dt;
	auto downBlock = world->GetCube(floor(nextPos.x + 0.5f), floor(nextPos.y), floor(nextPos.z + 0.5f));
	if (downBlock) {
		auto& blockData = BlockData::Get(*downBlock);
		if (!(blockData.flags & BF_NO_PHYSICS)) {
			velocityY = -5 * dt;
			if (kb.Space)
				velocityY = 10.0f;
		} else if (blockData.flags & BF_GRAVITY_WATER) {
			velocityY *= 0.7;
			if (kb.Space)
				velocityY = 10.0f;
		}
	}
	position += Vector3(0, velocityY, 0) * dt;

	for (auto colPoint : collisionPoints) {
		Vector3 colPos = position + colPoint + Vector3(0.5f, 0.5f, 0.5f);

		auto block = world->GetCube(floor(colPos.x), floor(colPos.y), floor(colPos.z));
		if (block) {
			auto& blockData = BlockData::Get(*block);
			if (blockData.flags & BF_NO_PHYSICS) continue;

			if (colPoint.x != 0)
				position.x += round(colPos.x) - colPos.x;
			if (colPoint.z != 0)
				position.z += round(colPos.z) - colPos.z;
			if (colPoint.y != 0 && colPoint.x == 0 && colPoint.z == 0)
				position.y += round(colPos.y) - colPos.y;
		}
	}

	camera.SetRotation(camRot);
	camera.SetPosition(position + Vector3(0, 1.25f, 0));
	highlightCube.model = Matrix::Identity;

	auto cubes = Raycast(camera.GetPosition() + Vector3(0.5, 0.5, 0.5), camera.Forward(), 5);
	for (int i = 0; i < cubes.size(); i++) {
		auto block = world->GetCube(cubes[i][0], cubes[i][1], cubes[i][2]);
		if (!block) continue;
		auto& blockData = BlockData::Get(*block);
		if (blockData.flags & BF_NO_RAYCAST) continue;

		highlightCube.model = Matrix::CreateTranslation(cubes[i][0], cubes[i][1], cubes[i][2]);
		if (mouseTracker.leftButton == ButtonState::PRESSED) {
			world->UpdateBlock(cubes[i][0], cubes[i][1], cubes[i][2], EMPTY);
		} else if(mouseTracker.rightButton == ButtonState::PRESSED && i > 0) {
			if (blockData.flags & BF_HALF_BLOCK && *block == currentCube.GetBlockId()) {
				world->UpdateBlock(cubes[i][0], cubes[i][1], cubes[i][2], (BlockId)((int)currentCube.GetBlockId() + 1));
			} else {
				world->UpdateBlock(cubes[i - 1][0], cubes[i - 1][1], cubes[i - 1][2], currentCube.GetBlockId());
			}
		}
		break;
	}

	if (ms.scrollWheelValue != 0) {
		int id = currentCube.GetBlockId();
		id = (id + signInt(ms.scrollWheelValue)) % COUNT;
		currentCube.SetBlockId((BlockId)id);
	}

}

void Player::Draw(DeviceResources* deviceRes) {
	auto gpuRes = DefaultResources::Get();

	gpuRes->noDepth.Apply(deviceRes);
	gpuRes->cbModel.ApplyToVS(deviceRes, 0);

	Matrix cubePos = Matrix::CreateTranslation(1.5,-1.5,-2) * camera.GetInverseViewMatrix();
	gpuRes->cbModel.data.model = cubePos.Transpose();
	gpuRes->cbModel.UpdateBuffer(deviceRes);
	currentCube.Draw(deviceRes);

	gpuRes->depthEqual.Apply(deviceRes);
	gpuRes->cbModel.data.model = highlightCube.model.Transpose();
	gpuRes->cbModel.UpdateBuffer(deviceRes);
	highlightCube.Draw(deviceRes);

	gpuRes->cbModel.data.model = Matrix::Identity;
	gpuRes->cbModel.UpdateBuffer(deviceRes);
	gpuRes->defaultDepth.Apply(deviceRes);
}
