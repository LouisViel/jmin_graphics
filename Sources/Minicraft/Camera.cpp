#include "pch.h"

#include "Camera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Camera::Camera(float fov, float aspectRatio) : fov(fov) {
	// TP initialiser matrices
}

Camera::~Camera() {
	if (cbCamera) delete cbCamera;
	cbCamera = nullptr;
}

void Camera::UpdateAspectRatio(float aspectRatio) {
	// TP updater matrice proj
}

void Camera::Update(float dt, Keyboard::State kb, Mouse* mouse) {
	float camSpeedRot = 0.25f;
	float camSpeedMouse = 10.0f;
	float camSpeed = 15.0f;
	if (kb.LeftShift) camSpeed *= 2.0f;

	Mouse::State mstate = mouse->GetState();
	const Matrix im = view.Invert();

	// TP: deplacement par clavier 

	// astuce: Vector3::TransformNormal(vecteur, im); transforme un vecteur de l'espace cameravers l'espace monde

    if (mstate.positionMode == Mouse::MODE_RELATIVE) {
		float dx = mstate.x;
		float dy = mstate.y;
        if (mstate.rightButton) { 
			// TP Translate camera a partir de dx/dy

        } else if (mstate.leftButton) {
			// TP Rotate camera a partir de dx/dy
        } else {
            mouse->SetMode(Mouse::MODE_ABSOLUTE);
        }
    } else if (mstate.rightButton || mstate.leftButton) {
        mouse->SetMode(Mouse::MODE_RELATIVE);
    }

	// TP updater matrice view
}

void Camera::ApplyCamera(DeviceResources* deviceRes) {
	if (!cbCamera) {
		cbCamera = new ConstantBuffer<MatrixData>();
		cbCamera->Create(deviceRes);
	}

	// TP envoyer data
}
