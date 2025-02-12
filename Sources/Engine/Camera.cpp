#include "pch.h"

#include "Camera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Camera::Camera() {
	view = Matrix::CreateLookAt(camPos, camPos + Vector3::Forward, Vector3::Up);
}

Camera::~Camera() {
	if (cbCamera) delete cbCamera;
	cbCamera = nullptr;
}

void Camera::UpdateViewMatrix() {
	Vector3 newForward = Vector3::Transform(Vector3::Forward, camRot);
	Vector3 newUp = Vector3::Transform(Vector3::Up, camRot);

	view = Matrix::CreateLookAt(camPos, camPos + newForward, newUp);

	BoundingFrustum::CreateFromMatrix(bounds, projection, true);
	bounds.Transform(bounds, view.Invert());
}

void Camera::ApplyCamera(DeviceResources* deviceRes) {
	if (!cbCamera) {
		cbCamera = new ConstantBuffer<MatrixData>();
		cbCamera->Create(deviceRes);
	}

	cbCamera->data.mView = view.Transpose();
	cbCamera->data.mProjection = projection.Transpose();
	cbCamera->UpdateBuffer(deviceRes);
	cbCamera->ApplyToVS(deviceRes, 1);
}

PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio) : fov(fov), Camera() {
	UpdateAspectRatio(aspectRatio);
}

void PerspectiveCamera::UpdateAspectRatio(float aspectRatio) {
	projection = Matrix::CreatePerspectiveFieldOfView(fov * XM_PI / 180.0f, aspectRatio, nearPlane, farPlane);
}

OrthographicCamera::OrthographicCamera(float width, float height) : width(width), height(height), Camera() {
	UpdateSize(width, height);
}

void OrthographicCamera::UpdateSize(float width, float height) {
	projection = Matrix::CreateOrthographic(width, height, nearPlane, farPlane);
}
