#pragma once

#include "Engine/Buffers.h"
#include "Engine/VertexLayout.h"
#include "Minicraft/World.h"
#include "Minicraft/Block.h"

#define CHUNK_SIZE 16
class World;
class Chunk {
	BlockId data[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
	World* world;

	VertexBuffer<VertexLayout_PositionUV> vb;
	IndexBuffer ib;

	Chunk* adjXPos = nullptr;
	Chunk* adjXNeg = nullptr;
	Chunk* adjYPos = nullptr;
	Chunk* adjYNeg = nullptr;
	Chunk* adjZPos = nullptr;
	Chunk* adjZNeg = nullptr;
public:
	Matrix model;
	DirectX::BoundingBox bounds;
	bool needRegen = false;

	Chunk(World* world, Vector3 pos);

	void Generate(DeviceResources* deviceRes);
	void Draw(DeviceResources* deviceRes);

	BlockId* GetCubeLocal(int lx, int ly, int lz);
private:
	void PushCube(int x, int y, int z);
	void PushFace(Vector3 pos, Vector3 up, Vector3 right, int id);
	bool ShouldRenderFace(int lx, int ly, int lz, int dx, int dy, int dz);

	friend class World;
};