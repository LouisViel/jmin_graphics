#include "pch.h"

#include "Chunk.h"
#include "Utils.h"

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

	float scaleY = (data.flags & BF_HALF_BLOCK) ? 0.5f : 1.0f;
	if (ShouldRenderFace(x, y, z, 0, 0, 1)) PushFace({ -0.5f + x, -0.5f + y, 0.5f + z }, Vector3::Up, Vector3::Right, Vector3::Backward, data.texIdSide, data.pass, scaleY);
	if (ShouldRenderFace(x, y, z, 1, 0, 0)) PushFace({ 0.5f + x, -0.5f + y, 0.5f + z }, Vector3::Up, Vector3::Forward, Vector3::Right, data.texIdSide, data.pass, scaleY);
	if (ShouldRenderFace(x, y, z, 0, 0,-1)) PushFace({ 0.5f + x, -0.5f + y,-0.5f + z }, Vector3::Up, Vector3::Left, Vector3::Forward, data.texIdSide, data.pass, scaleY);
	if (ShouldRenderFace(x, y, z,-1, 0, 0)) PushFace({ -0.5f + x, -0.5f + y,-0.5f + z }, Vector3::Up, Vector3::Backward, Vector3::Left, data.texIdSide, data.pass, scaleY);
	if (scaleY != 1.0f || ShouldRenderFace(x, y, z, 0, 1, 0)) PushFace({ -0.5f + x, (scaleY - 0.5f) + y, 0.5f + z }, Vector3::Forward, Vector3::Right, Vector3::Up, data.texIdTop, data.pass);
	if (ShouldRenderFace(x, y, z, 0,-1, 0)) PushFace({ -0.5f + x, -0.5f + y,-0.5f + z }, Vector3::Backward, Vector3::Right, Vector3::Down, data.texIdBottom, data.pass);
}

void Chunk::PushFace(Vector3 pos, Vector3 up, Vector3 right, Vector3 normal, int id, ShaderPass pass, float scaleY) {
	Vector2 uv(
		(id % 16) * BLOCK_TEXSIZE,
		(id / 16) * BLOCK_TEXSIZE
	);

	auto a = vb[pass].PushVertex({ ToVec4(pos), ToVec4Normal(normal), uv + Vector2::UnitY * BLOCK_TEXSIZE * scaleY });
	auto b = vb[pass].PushVertex({ ToVec4(pos + up * scaleY), ToVec4Normal(normal), uv });
	auto c = vb[pass].PushVertex({ ToVec4(pos + right), ToVec4Normal(normal), uv + Vector2::UnitX * BLOCK_TEXSIZE + Vector2::UnitY * BLOCK_TEXSIZE * scaleY });
	auto d = vb[pass].PushVertex({ ToVec4(pos + up * scaleY + right), ToVec4Normal(normal), uv + Vector2::UnitX * BLOCK_TEXSIZE });
	ib[pass].PushTriangle(a, b, c);
	ib[pass].PushTriangle(c, b, d);
}

bool Chunk::ShouldRenderFace(int lx, int ly, int lz, int dx, int dy, int dz) {
	auto neighbour = GetCubeLocal(lx + dx, ly + dy, lz + dz);
	if (!neighbour) return true;
	auto myself = GetCubeLocal(lx, ly, lz);

	const BlockData& myData = BlockData::Get(*myself);
	const BlockData& neighData = BlockData::Get(*neighbour);

	if (neighData.flags & BF_HALF_BLOCK)
		return true;

	if (neighData.flags & BF_CUTOUT)
		return !(myData.flags & BF_CUTOUT);

	bool isNeighTransp = neighData.pass == SP_TRANSPARENT;
	if (isNeighTransp) {
		bool isTransp = myData.pass == SP_TRANSPARENT;
		return !isTransp;
	}

	return *neighbour == EMPTY;
}

void Chunk::Generate(DeviceResources* deviceRes) {
	for (int pass = SP_OPAQUE; pass < SP_COUNT; pass++) {
		vb[pass].Clear();
		ib[pass].Clear();
	}

	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			for (int y = 0; y < CHUNK_SIZE; y++) {
				auto block = GetCubeLocal(x, y, z);
				if (EMPTY == *block) continue;
				PushCube(x, y, z);
			}
		}
	}

	for (int pass = SP_OPAQUE; pass < SP_COUNT; pass++) {
		vb[pass].Create(deviceRes);
		ib[pass].Create(deviceRes);
	}
	needRegen = false;
}

void Chunk::Draw(DeviceResources* deviceRes, ShaderPass pass) {
	if (vb[pass].Size() == 0) return;
	vb[pass].Apply(deviceRes, 0);
	ib[pass].Apply(deviceRes);
	deviceRes->GetD3DDeviceContext()->DrawIndexed(ib[pass].Size(), 0, 0);
}