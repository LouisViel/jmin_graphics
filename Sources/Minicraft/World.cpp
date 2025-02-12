#include "pch.h"

#include "Engine/DefaultResources.h"
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

World::~World() {
	for (int idx = 0; idx < WORLD_SIZE * WORLD_SIZE * WORLD_HEIGHT; idx++) {
		delete chunks[idx];
		chunks[idx] = nullptr;
	}
}

float scaleHuge = 50;
float intensityHuge = 20;
float scaleMedium = 10;
float intensityMedium = 5;
int waterHeight = 12;

void World::Generate(DeviceResources* deviceRes) {
	siv::BasicPerlinNoise<float> perlin;
	for (int x = 0; x < CHUNK_SIZE * WORLD_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE * WORLD_SIZE; z++) {
			int stoneLayer = 2 + floor(perlin.noise2D_01(x / scaleHuge, z / scaleHuge) * intensityHuge);
			for (int y = 0; y < stoneLayer; y++) {
				auto block = GetCube(x, y, z);
				*block = STONE;
			}

			int dirtLayer = stoneLayer + 1 + floor(perlin.noise2D_01(x / scaleMedium, z / scaleMedium) * intensityMedium);
			for (int y = stoneLayer; y < dirtLayer; y++) {
				auto block = GetCube(x, y, z);
				*block = DIRT;
			}

			for (int y = dirtLayer; y < waterHeight; y++) {
				auto block = GetCube(x, y, z);
				*block = WATER;
			}

			if (dirtLayer > waterHeight - 1) {
				auto block = GetCube(x, dirtLayer - 1, z);
				*block = GRASS;
			}
		}
	}

	for (int idx = 0; idx < WORLD_SIZE * WORLD_SIZE * WORLD_HEIGHT; idx++)
		chunks[idx]->Generate(deviceRes);

	DefaultResources::Get()->cbModel.Create(deviceRes);
}

void World::Draw(Camera* camera, DeviceResources* deviceRes) {
	auto gpuRes = DefaultResources::Get();
	gpuRes->cbModel.ApplyToVS(deviceRes, 0);

	for (int pass = SP_OPAQUE; pass < SP_COUNT; pass++) {
		switch (pass) {
		case SP_OPAQUE:
			gpuRes->opaque.Apply(deviceRes);
			gpuRes->defaultDepth.Apply(deviceRes);
			break;
		case SP_TRANSPARENT:
			gpuRes->alphaBlend.Apply(deviceRes);
			gpuRes->depthRead.Apply(deviceRes);
			break;
		}

		for (int idx = 0; idx < WORLD_SIZE * WORLD_SIZE * WORLD_HEIGHT; idx++) {
			if (chunks[idx]->needRegen) chunks[idx]->Generate(deviceRes);

			if (chunks[idx]->bounds.Intersects(camera->bounds)) {
				gpuRes->cbModel.data.model = chunks[idx]->model.Transpose();
				gpuRes->cbModel.UpdateBuffer(deviceRes);
				chunks[idx]->Draw(deviceRes, (ShaderPass)pass);
			}
		}
	}
	gpuRes->cbModel.data.model = Matrix::Identity;
	gpuRes->cbModel.UpdateBuffer(deviceRes);
}

Chunk* World::GetChunk(int cx, int cy, int cz) {
	if (cx < 0 || cy < 0 || cz < 0) return nullptr;
	if (cx > WORLD_SIZE - 1 || cy > WORLD_SIZE - 1 || cz > WORLD_SIZE - 1) return nullptr;
	return chunks[cx + cy * WORLD_SIZE + cz * WORLD_SIZE * WORLD_HEIGHT];
}

BlockId* World::GetCube(int gx, int gy, int gz) {
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

void World::MakeChunkDirty(int gx, int gy, int gz) {
	auto chunk = GetChunkFromCoordinates(gx, gy, gz);
	if (chunk) chunk->needRegen = true;
}

Chunk* World::GetChunkFromCoordinates(int gx, int gy, int gz) {
	int cx = gx / CHUNK_SIZE;
	int cy = gy / CHUNK_SIZE;
	int cz = gz / CHUNK_SIZE;
	return GetChunk(cx, cy, cz);
}

void World::UpdateBlock(int gx, int gy, int gz, BlockId block) {
	auto cube = GetCube(gx, gy, gz);
	if (!cube) return;
	*cube = block;

	MakeChunkDirty(gx, gy, gz);
	MakeChunkDirty(gx + 1, gy, gz);
	MakeChunkDirty(gx - 1, gy, gz);
	MakeChunkDirty(gx, gy + 1, gz);
	MakeChunkDirty(gx, gy - 1, gz);
	MakeChunkDirty(gx, gy, gz + 1);
	MakeChunkDirty(gx, gy, gz - 1);
}