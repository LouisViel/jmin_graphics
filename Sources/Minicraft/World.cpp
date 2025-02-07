#include "pch.h"

#include "World.h"
#include "PerlinNoise.hpp"

World::World() {
	for (int x = 0; x < WORLD_SIZE; x++) {
		for (int y = 0; y < WORLD_HEIGHT; y++) {
			for (int z = 0; z < WORLD_SIZE; z++) {
				Chunk* c = new Chunk(this, Vector3(x, y, z) * CHUNK_SIZE);
				chunks[x + y * WORLD_SIZE + z * WORLD_SIZE * WORLD_HEIGHT] = c;
			}
		}
	}
	for (int x = 0; x < WORLD_SIZE; x++) {
		for (int y = 0; y < WORLD_HEIGHT; y++) {
			for (int z = 0; z < WORLD_SIZE; z++) {
				auto chunk = GetChunk(x, y, z);
				if (x > 0) chunk->adjXNeg = GetChunk(x - 1, y, z);
				if (y > 0) chunk->adjYNeg = GetChunk(x, y - 1, z);
				if (z > 0) chunk->adjZNeg = GetChunk(x, y, z - 1);
				if (x < WORLD_SIZE - 1) chunk->adjXPos = GetChunk(x + 1, y, z);
				if (y < WORLD_SIZE - 1) chunk->adjYPos = GetChunk(x, y + 1, z);
				if (z < WORLD_SIZE - 1) chunk->adjZPos = GetChunk(x, y, z + 1);
			}
		}
	}
}

void World::Generate(DeviceResources* deviceRes) {
	siv::BasicPerlinNoise<float> perlin;
	for (int x = 0; x < CHUNK_SIZE * WORLD_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE * WORLD_SIZE; z++) {
			// perlin.noise2D_01(x, z);
			for (int y = 0; y < 5; y++) {
				auto block = GetCubes(x, y, z);
				*block = DIRT;
			}
			auto block = GetCubes(x, 5, z);
			*block = GRASS;
		}
	}

	for (int idx = 0; idx < WORLD_SIZE * WORLD_SIZE * WORLD_HEIGHT; idx++)
		chunks[idx]->Generate(deviceRes);

	constantBufferModel.Create(deviceRes);
}

void World::Draw(DeviceResources* deviceRes) {
	constantBufferModel.ApplyToVS(deviceRes, 0);

	for (int idx = 0; idx < WORLD_SIZE * WORLD_SIZE * WORLD_HEIGHT; idx++) {
		constantBufferModel.data.model = chunks[idx]->model.Transpose();
		constantBufferModel.UpdateBuffer(deviceRes);
		chunks[idx]->Draw(deviceRes);
	}
}

Chunk* World::GetChunk(int cx, int cy, int cz) {
	if (cx < 0 || cy < 0 || cz < 0) return nullptr;
	if (cx > WORLD_SIZE - 1 || cy > WORLD_SIZE - 1 || cz > WORLD_SIZE - 1) return nullptr;
	return chunks[cx + cy * WORLD_SIZE + cz * WORLD_SIZE * WORLD_HEIGHT];
}

BlockId* World::GetCubes(int gx, int gy, int gz) {
	int cx = gx / CHUNK_SIZE;
	int cy = gy / CHUNK_SIZE;
	int cz = gz / CHUNK_SIZE;
	int lx = gx % CHUNK_SIZE;
	int ly = gy % CHUNK_SIZE;
	int lz = gz % CHUNK_SIZE;

	auto chunk = GetChunk(cx, cy, cz);
	if (!chunk) return nullptr;
	return chunk->GetCubeLocal(lx, ly, lz);
}