#pragma once

#include "Engine/BlendState.h"
#include "Engine/Camera.h"
#include "Minicraft/Block.h"
#include "Minicraft/Chunk.h"

#define WORLD_SIZE 15
#define WORLD_HEIGHT 3

class Chunk;
class World {
	Chunk* chunks[WORLD_SIZE * WORLD_HEIGHT * WORLD_SIZE];
public:
	World();
	virtual ~World();
	void Generate(DeviceResources* deviceRes);
	void Draw(Camera* camera, DeviceResources* deviceRes);

	Chunk* GetChunk(int cx, int cy, int cz);
	Chunk* GetChunkFromCoordinates(int gx, int gy, int gz);
	BlockId* GetCube(int gx, int gy, int gz);
	void MakeChunkDirty(int gx, int gy, int gz);

	void UpdateBlock(int gx, int gy, int gz, BlockId block);

	friend class Chunk;
};