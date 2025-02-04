#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class MeshHelper
{
public:
	static void PushCW(std::vector<float>& vertexs, std::vector<uint32_t>& indexs, Vector3 pos, float size = 1.0f);
	static void PushCube(std::vector<float>& vertexs, std::vector<uint32_t>& indexs, Vector3 pos, Vector3 forward, Vector3 up, Vector3 right, float size = 1.0f);
	static void PushFace(std::vector<float>& vertexs, std::vector<uint32_t>& indexs, Vector3 pos, Vector3 up, Vector3 right, float size = 1.0f);

private:
	static inline void PushVertex(std::vector<float>& vertexs, Vector3 pos);
};