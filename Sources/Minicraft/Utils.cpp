#include "pch.h"

#include "Utils.h"

Vector4 ToVec4(const Vector3& v) {
	return Vector4(v.x, v.y, v.z, 1.0f);
}

Vector4 ToVec4Normal(const Vector3& v) {
	return Vector4(v.x, v.y, v.z, 0.0f);
}

float sign(float v) {
	if (v < 0) return -1;
	else return 1;
}

int signInt(int v) {
	if (v < 0) return -1;
	else return 1;
}

std::vector<std::array<int, 3>> Raycast(Vector3 pos, Vector3 dir, float maxDist) {
	std::map<float, std::array<int, 3>> cubes;

	if (dir.x != 0) {
		float deltaYX = dir.y / dir.x;
		float deltaZX = dir.z / dir.x;
		float offsetYX = pos.y - pos.x * deltaYX;
		float offsetZX = pos.z - pos.x * deltaZX;

		float cubeX = (dir.x > 0) ? ceil(pos.x) : floor(pos.x);
		do {
			Vector3 collision(cubeX, deltaYX * cubeX + offsetYX, deltaZX * cubeX + offsetZX);
			float dist = Vector3::Distance(pos, collision);
			if (dist > maxDist) break;

			cubes[dist] = {
				(int)floor(cubeX - ((dir.x < 0) ? 1 : 0)),
				(int)floor(collision.y),
				(int)floor(collision.z)
			};
			cubeX = cubeX + sign(dir.x);
		} while (true);
	}
	if (dir.y != 0) {
		float deltaXY = dir.x / dir.y;
		float deltaZY = dir.z / dir.y;
		float offsetXY = pos.x - pos.y * deltaXY;
		float offsetZY = pos.z - pos.y * deltaZY;

		float cubeY = (dir.y > 0) ? ceil(pos.y) : floor(pos.y);
		do {
			Vector3 collision(deltaXY * cubeY + offsetXY, cubeY, deltaZY * cubeY + offsetZY);
			float dist = Vector3::Distance(pos, collision);
			if (dist > maxDist) break;

			cubes[dist] = {
				(int)floor(collision.x),
				(int)floor(cubeY - ((dir.y < 0) ? 1 : 0)),
				(int)floor(collision.z)
			};
			cubeY = cubeY + sign(dir.y);
		} while (true);
	}
	if (dir.z != 0) {
		float deltaXZ = dir.x / dir.z;
		float deltaYZ = dir.y / dir.z;
		float offsetXZ = pos.x - pos.z * deltaXZ;
		float offsetYZ = pos.y - pos.z * deltaYZ;

		float cubeZ = (dir.z > 0) ? ceil(pos.z) : floor(pos.z);
		do {
			Vector3 collision(deltaXZ * cubeZ + offsetXZ, deltaYZ * cubeZ + offsetYZ, cubeZ);
			float dist = Vector3::Distance(pos, collision);
			if (dist > maxDist) break;

			cubes[dist] = {
				(int)floor(collision.x),
				(int)floor(collision.y),
				(int)floor(cubeZ - ((dir.z < 0) ? 1 : 0)),
			};
			cubeZ = cubeZ + sign(dir.z);
		} while (true);
	}

	std::vector<std::array<int, 3>> res;
	std::transform(
		cubes.begin(), cubes.end(),
		std::back_inserter(res),
		[](auto& v) { return v.second; });
	return res;
}
