#include "Camera.h"

Camera::Camera()
{
	this->cameraPerspective;
	this->cameraProjection;
	this->position = { 0.0f,0.0f,0.0f };
	this->direction = { 0.0f,0.0f,0.0f };
	this->lookAt = { 0.0f,0.0f,0.0f };
	this->upVector = { 0.0f, 0.0f, 0.0f };
}

Camera::Camera(XMFLOAT3 position, XMFLOAT3 direction)
{
	this->cameraPerspective;
	this->cameraProjection;
	this->position = position;
	this->direction = direction;
	setupCam();
}

Camera::~Camera()
{
}

void Camera::setupCam()
{
	this->cameraPerspective = DirectX::XMMatrixLookAtLH(XMVectorSet(this->position.x, this->position.y, this->position.z, 0), XMVectorSet(this->direction.x, this->direction.y, this->direction.z, 0), XMVectorSet(0,1,0,0)); //skapar vänster-orienterat koordinatsystem,
	this->cameraProjection = DirectX::XMMatrixPerspectiveFovLH(0.45f * 3.14f, (float)640 / 480, 0.1f, 20.0f); //projicering matris baserad på field of view, 
}
