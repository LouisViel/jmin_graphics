#include "pch.h"
#include "MeshHelper.h"

void MeshHelper::PushCW(
	std::vector<float>& vertexs, std::vector<uint32_t>& indexs,
	Vector3 pos, float size
) {
	PushCube(vertexs, indexs, pos, Vector3::Forward, Vector3::Right, Vector3::Up, size);
}

void MeshHelper::PushCube(
	std::vector<float>& vertexs, std::vector<uint32_t>& indexs,
	Vector3 pos, Vector3 up, Vector3 right, Vector3 forward,
	float size
) {
	float halfSize = size * 0.5f;
	Vector3 hup = halfSize * up;
	Vector3 hright = halfSize * right;
	Vector3 hforward = halfSize * forward;

	PushFace(vertexs, indexs, pos + hup, -forward, right, size); // Top
	PushFace(vertexs, indexs, pos - hup, forward, right, size); // Bottom
	PushFace(vertexs, indexs, pos + hforward, up, right, size); // Front
	PushFace(vertexs, indexs, pos - hforward, up, -right, size); // Back
	PushFace(vertexs, indexs, pos + hright, up, -forward, size); // Right
	PushFace(vertexs, indexs, pos - hright, up, forward, size); // Left
}


void MeshHelper::PushFace(
	std::vector<float>& vertexs, std::vector<uint32_t>& indexs,
	Vector3 pos, Vector3 up, Vector3 right,
	float size
) {
	// Pre-Process
	uint32_t vIndex = vertexs.size();
	float halfSize = size * 0.5f;
	Vector3 hup = halfSize * up;
	Vector3 hright = halfSize * right;

	// Process Vertex Positions
	PushVertex(vertexs, pos + hup + hright);
	PushVertex(vertexs, pos + hup - hright);
	PushVertex(vertexs, pos - hup - hright);
	PushVertex(vertexs, pos - hup + hright);

	// Process Vertex Indexs (Part 1)
	indexs.insert(indexs.end(), {
		vIndex + 1, vIndex, vIndex + 2,
		vIndex + 2, vIndex, vIndex + 3
	});
}

inline void MeshHelper::PushVertex(std::vector<float>& vertexs, Vector3 vpos)
{
	vertexs.push_back(vpos.x);
	vertexs.push_back(vpos.y);
	vertexs.push_back(vpos.z);
}