#include "pch.h"

#include "Cube.h"

Cube::Cube(Vector3 pos) {
	model = Matrix::CreateTranslation(pos);
}

void Cube::Generate(DeviceResources* deviceRes) {
	PushFace({ -0.5f, -0.5f, 0.5f }, Vector3::Up, Vector3::Right);
	PushFace({  0.5f, -0.5f, 0.5f }, Vector3::Up, Vector3::Forward);
	PushFace({  0.5f, -0.5f,-0.5f }, Vector3::Up, Vector3::Left);
	PushFace({ -0.5f, -0.5f,-0.5f }, Vector3::Up, Vector3::Backward);
	PushFace({ -0.5f,  0.5f, 0.5f }, Vector3::Forward, Vector3::Right);
	PushFace({ -0.5f, -0.5f,-0.5f }, Vector3::Backward, Vector3::Right);
	vb.Create(deviceRes);
	ib.Create(deviceRes);
}

void Cube::Draw(DeviceResources* deviceRes) {
	vb.Apply(deviceRes, 0);
	ib.Apply(deviceRes);
	deviceRes->GetD3DDeviceContext()->DrawIndexed(ib.Size(), 0, 0);
}

Vector4 ToVec4(const Vector3& v) {
	return Vector4(v.x, v.y, v.z, 1.0f);
}

void Cube::PushFace(Vector3 pos, Vector3 up, Vector3 right) {
	auto a = vb.PushVertex({ ToVec4(pos), {0,0} });
	auto b = vb.PushVertex({ ToVec4(pos + up), {0,1} });
	auto c = vb.PushVertex({ ToVec4(pos + right), {1,0} });
	auto d = vb.PushVertex({ ToVec4(pos + up + right), {1,1} });
	ib.PushTriangle(a, b, c);
	ib.PushTriangle(c, b, d);
}
