#include "pch.h"

#include "Cube3D.h"
#include "Utils.h"

void Cube3D::PushFace(Vector3 pos, Vector3 up, Vector3 right, Vector3 normal, int id) {
	Vector2 uv(
		(id % 16) * BLOCK_TEXSIZE,
		(id / 16) * BLOCK_TEXSIZE
	);

	auto a = vb.PushVertex({ ToVec4(pos), ToVec4Normal(normal), uv + Vector2::UnitY * BLOCK_TEXSIZE });
	auto b = vb.PushVertex({ ToVec4(pos + up), ToVec4Normal(normal), uv });
	auto c = vb.PushVertex({ ToVec4(pos + right), ToVec4Normal(normal), uv + Vector2::UnitX * BLOCK_TEXSIZE + Vector2::UnitY * BLOCK_TEXSIZE });
	auto d = vb.PushVertex({ ToVec4(pos + up + right), ToVec4Normal(normal), uv + Vector2::UnitX * BLOCK_TEXSIZE });
	ib.PushTriangle(a, b, c);
	ib.PushTriangle(c, b, d);
}

void Cube3D::Generate(DeviceResources* deviceRes) {
	vb.Clear();
	ib.Clear();

	auto& data = BlockData::Get(blockId);
	float uvxSide = (data.texIdSide % 16) / BLOCK_TEXSIZE;
	float uvySide = (data.texIdSide / 16) / BLOCK_TEXSIZE;

	PushFace({ -0.5f, -0.5f,  0.5f }, Vector3::Up, Vector3::Right, Vector3::Backward, data.texIdSide);
	PushFace({  0.5f, -0.5f,  0.5f }, Vector3::Up, Vector3::Forward, Vector3::Right, data.texIdSide);
	PushFace({  0.5f, -0.5f, -0.5f }, Vector3::Up, Vector3::Left, Vector3::Forward, data.texIdSide);
	PushFace({ -0.5f, -0.5f, -0.5f }, Vector3::Up, Vector3::Backward, Vector3::Left, data.texIdSide);
	PushFace({ -0.5f,  0.5f,  0.5f }, Vector3::Forward, Vector3::Right, Vector3::Up, data.texIdTop);
	PushFace({ -0.5f, -0.5f, -0.5f }, Vector3::Backward, Vector3::Right, Vector3::Down, data.texIdBottom);

	vb.Create(deviceRes);
	ib.Create(deviceRes);

	needRegen = false;
}

void Cube3D::Draw(DeviceResources* deviceRes) {
	if (needRegen)
		Generate(deviceRes);

	if (vb.Size() == 0) return;
	vb.Apply(deviceRes, 0);
	ib.Apply(deviceRes);
	deviceRes->GetD3DDeviceContext()->DrawIndexed(ib.Size(), 0, 0);
}