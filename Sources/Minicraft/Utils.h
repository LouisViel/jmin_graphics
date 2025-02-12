#pragma once

using namespace DirectX::SimpleMath;

Vector4 ToVec4(const Vector3& v);
Vector4 ToVec4Normal(const Vector3& v);

float sign(float v);
int signInt(int v);
std::vector<std::array<int, 3>> Raycast(Vector3 pos, Vector3 dir, float maxDist);