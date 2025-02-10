#pragma once

#include "Minicraft/Block.h"
#include "Minicraft/Camera.h"
#include "Minicraft/Chunk.h"

#define WORLD_SIZE 5
#define WORLD_HEIGHT 3

class Chunk;
class World {
	Chunk* chunks[WORLD_SIZE * WORLD_HEIGHT * WORLD_SIZE];

	struct ModelData {
		Matrix model;
	};
	ConstantBuffer<ModelData> constantBufferModel;
public:
	World();
	virtual ~World();
	void Generate(DeviceResources* deviceRes);
	void Draw(Camera* camera, DeviceResources* deviceRes);

	Chunk* GetChunk(int cx, int cy, int cz);
	Chunk* GetChunkFromCoordinates(int gx, int gy, int gz);
	BlockId* GetCubes(int gx, int gy, int gz);
	void MakeChunkDirty(int gx, int gy, int gz);

	void UpdateBlock(int gx, int gy, int gz, BlockId block);

	friend class Chunk;
};