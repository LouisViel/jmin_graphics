#include "pch.h"

#include "Camera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Camera::Camera(float fov, float aspectRatio) : fov(fov) {
	UpdateAspectRatio(aspectRatio);
	view = Matrix::CreateLookAt(camPos, camPos + Vector3::Forward, Vector3::Up);
}

Camera::~Camera() {
	if (cbCamera) delete cbCamera;
	cbCamera = nullptr;
}

void Camera::UpdateAspectRatio(float aspectRatio) {
	projection = Matrix::CreatePerspectiveFieldOfView(fov * XM_PI / 180.0f, aspectRatio, nearPlane, farPlane);
}

void Camera::Update(float dt, Keyboard::State kb, Mouse* mouse) {
	float camSpeedRot = 0.25f;
	float camSpeedMouse = 10.0f;
	float camSpeed = 15.0f;
	if (kb.LeftShift) camSpeed *= 2.0f;

	Mouse::State mstate = mouse->GetState();
	const Matrix viewInverse = view.Invert();

	Vector3 delta;
	if (kb.Z) delta += Vector3::Forward;
	if (kb.S) delta += Vector3::Backward;
	if (kb.Q) delta += Vector3::Left;
	if (kb.D) delta += Vector3::Right;
	camPos += Vector3::TransformNormal(delta, viewInverse) * camSpeed * dt;

    if (mstate.positionMode == Mouse::MODE_RELATIVE) {
		float dx = mstate.x;
		float dy = mstate.y;
        if (mstate.rightButton) {
			Vector3 deltaMouse;
			if(kb.LeftShift || kb.RightShift)
				deltaMouse = Vector3(0, 0, dy);
			else
				deltaMouse = Vector3(-dx, dy, 0);
			camPos += Vector3::TransformNormal(deltaMouse, viewInverse) * camSpeed * dt;
			// TP Translate camera a partir de dx/dy
        } else if (mstate.leftButton) {
			camRot *= Quaternion::CreateFromAxisAngle(Vector3::TransformNormal(Vector3::Right, viewInverse), -dy * dt);
			camRot *= Quaternion::CreateFromAxisAngle(Vector3::Up, -dx * dt);
        } else {
            mouse->SetMode(Mouse::MODE_ABSOLUTE);
        }
    } else if (mstate.rightButton || mstate.leftButton) {
        mouse->SetMode(Mouse::MODE_RELATIVE);
    }

	Vector3 newForward = Vector3::Transform(Vector3::Forward, camRot);
	Vector3 newUp = Vector3::Transform(Vector3::Up, camRot);

	view = Matrix::CreateLookAt(camPos, camPos + newForward, newUp);
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
