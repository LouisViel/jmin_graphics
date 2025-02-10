#include "pch.h"

#include "Chunk.h"

Vector4 ToVec4(const Vector3& v) {
	return Vector4(v.x, v.y, v.z, 1.0f);
}

Chunk::Chunk(World* world, Vector3 pos) {
	memset(data, EMPTY, sizeof(data));

	this->world = world;
	model = Matrix::CreateTranslation(pos);
	bounds = DirectX::BoundingBox(pos + Vector3(CHUNK_SIZE / 2 - 0.5, CHUNK_SIZE / 2 - 0.5, CHUNK_SIZE / 2 - 0.5), Vector3(CHUNK_SIZE / 2, CHUNK_SIZE / 2, CHUNK_SIZE / 2));
}

BlockId* Chunk::GetCubeLocal(int lx, int ly, int lz) {
	if (lx < 0) return adjXNeg ? adjXNeg->GetCubeLocal(CHUNK_SIZE - 1, ly, lz) : nullptr;
	if (ly < 0) return adjYNeg ? adjYNeg->GetCubeLocal(lx, CHUNK_SIZE - 1, lz) : nullptr;
	if (lz < 0) return adjZNeg ? adjZNeg->GetCubeLocal(lx, ly, CHUNK_SIZE - 1) : nullptr;
	if (lx >= CHUNK_SIZE) return adjXPos ? adjXPos->GetCubeLocal(0, ly, lz) : nullptr;
	if (ly >= CHUNK_SIZE) return adjYPos ? adjYPos->GetCubeLocal(lx, 0, lz) : nullptr;
	if (lz >= CHUNK_SIZE) return adjZPos ? adjZPos->GetCubeLocal(lx, ly, 0) : nullptr;
	return &data[lx + ly * CHUNK_SIZE + lz * CHUNK_SIZE * CHUNK_SIZE];
}

void Chunk::PushCube(int x, int y, int z) {
	auto blockId = GetCubeLocal(x, y, z);

	auto& data = BlockData::Get(*blockId);
	float uvxSide = (data.texIdSide % 16) / BLOCK_TEXSIZE;
	float uvySide = (data.texIdSide / 16) / BLOCK_TEXSIZE;

	if (ShouldRenderFace(x, y, z, 0, 0, 1)) PushFace({ -0.5f + x, -0.5f + y, 0.5f + z }, Vector3::Up, Vector3::Right, data.texIdSide);
	if (ShouldRenderFace(x, y, z, 1, 0, 0)) PushFace({ 0.5f + x, -0.5f + y, 0.5f + z }, Vector3::Up, Vector3::Forward, data.texIdSide);
	if (ShouldRenderFace(x, y, z, 0, 0,-1)) PushFace({ 0.5f + x, -0.5f + y,-0.5f + z }, Vector3::Up, Vector3::Left, data.texIdSide);
	if (ShouldRenderFace(x, y, z,-1, 0, 0)) PushFace({ -0.5f + x, -0.5f + y,-0.5f + z }, Vector3::Up, Vector3::Backward, data.texIdSide);
	if (ShouldRenderFace(x, y, z, 0, 1, 0)) PushFace({ -0.5f + x,  0.5f + y, 0.5f + z }, Vector3::Forward, Vector3::Right, data.texIdTop);
	if (ShouldRenderFace(x, y, z, 0,-1, 0)) PushFace({ -0.5f + x, -0.5f + y,-0.5f + z }, Vector3::Backward, Vector3::Right, data.texIdBottom);
}

void Chunk::PushFace(Vector3 pos, Vector3 up, Vector3 right, int id) {
	Vector2 uv(
		(id % 16) * BLOCK_TEXSIZE,
		(id / 16) * BLOCK_TEXSIZE
	);

	auto a = vb.PushVertex({ ToVec4(pos), uv + Vector2::UnitY * BLOCK_TEXSIZE });
	auto b = vb.PushVertex({ ToVec4(pos + up), uv });
	auto c = vb.PushVertex({ ToVec4(pos + right), uv + Vector2::UnitX * BLOCK_TEXSIZE + Vector2::UnitY * BLOCK_TEXSIZE });
	auto d = vb.PushVertex({ ToVec4(pos + up + right), uv + Vector2::UnitX * BLOCK_TEXSIZE });
	ib.PushTriangle(a, b, c);
	ib.PushTriangle(c, b, d);
}

bool Chunk::ShouldRenderFace(int lx, int ly, int lz, int dx, int dy, int dz) {
	auto neighbour = GetCubeLocal(lx + dx, ly + dy, lz + dz);
	if (!neighbour) return true;
	// todo manage transp
	return *neighbour == EMPTY;
}

void Chunk::Generate(DeviceResources* deviceRes) {
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			for (int y = 0; y < CHUNK_SIZE / 2; y++) {
				auto block = GetCubeLocal(x, y, z);
				if (EMPTY == *block) continue;
				PushCube(x, y, z);
			}
		}
	}

	if (vb.Size() != 0)
		vb.Create(deviceRes);
	if (ib.Size() != 0)
		ib.Create(deviceRes);
	needRegen = false;
}

void Chunk::Draw(DeviceResources* deviceRes) {
	if (vb.Size() == 0) return;
	vb.Apply(deviceRes, 0);
	ib.Apply(deviceRes);
	deviceRes->GetD3DDeviceContext()->DrawIndexed(ib.Size(), 0, 0);
}