#pragma once

#include "Engine/Buffers.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Camera {
protected:
	float nearPlane = 0.01f;
	float farPlane = 500.0f;
	
	Vector3 camPos = Vector3(0, 0, 2);
	Quaternion camRot = Quaternion();
	Matrix projection;
	Matrix view;

	struct MatrixData {
		Matrix mView;
		Matrix mProjection;
	};
	ConstantBuffer<MatrixData>* cbCamera = nullptr;

	void UpdateViewMatrix();
public:
	DirectX::BoundingFrustum bounds;

	Camera();
	virtual ~Camera();

	Vector3 GetPosition() const { return camPos; };
	Quaternion GetRotation() const { return camRot; };
	void SetPosition(const Vector3& pos) { camPos = pos; UpdateViewMatrix(); };
	void SetRotation(const Quaternion& rot) { camRot = rot; UpdateViewMatrix(); };

	Vector3 Forward() const { return Vector3::TransformNormal(Vector3::Forward, view.Invert()); }
	Vector3 Up() const { return Vector3::TransformNormal(Vector3::Up, view.Invert()); }
	Vector3 Right() const { return Vector3::TransformNormal(Vector3::Right, view.Invert()); }

	Matrix GetViewMatrix() const { return view; }
	Matrix GetInverseViewMatrix() const { return view.Invert(); }

	void ApplyCamera(DeviceResources* deviceRes);
};

class PerspectiveCamera : public Camera {
	float fov;
public:
	PerspectiveCamera(float fov, float aspectRatio);

	void UpdateAspectRatio(float aspectRatio);
};

class OrthographicCamera : public Camera {
	float width, height;
public:
	OrthographicCamera(float width, float height);

	void UpdateSize(float width, float height);
};