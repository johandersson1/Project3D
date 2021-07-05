#include "Camera.h"

Camera::Camera()
{
	this->cameraPerspective;
	this->cameraProjection;
	this->position = { 0.0f,0.0f,0.0f };
	this->direction = { 0.0f,0.0f,0.0f };
	this->lookAt = { 0.0f,0.0f,0.0f };
	this->upVector = { 0.0f, 0.0f, 0.0f };
	this->camRotationMatrix;
	this->speed = 0.003f;
	this->moveBackForward = 0.0f;
	this->moveLeftRight = 0.0f;
	this->camYaw = 0.0f;
	this->camPitch = 0.0f;
	this->camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	this->defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	this->camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	this->defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
}

Camera::Camera(XMFLOAT3 position, XMFLOAT3 direction, float speed)
{
	this->cameraPerspective;
	this->cameraProjection;
	this->position = position;
	this->direction = direction;
	this->lookAt = { 0.0f,0.0f,0.0f };
	this->upVector = { 0.0f, 0.0f, 0.0f };
	this->camRotationMatrix;
	this->speed = 0.003f;
	this->moveBackForward = 0.0f;
	this->moveLeftRight = 0.0f;
	this->camYaw = 0.0f;
	this->camPitch = 0.0f;
	this->camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	this->defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	this->camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	this->defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);


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

void Camera::detectInput(float movementSpeed, float lookSpeed)
{
	if (GetAsyncKeyState('W'))
	{
		moveCamera(XMFLOAT3(0, 0, 1), movementSpeed);
	}
	if (GetAsyncKeyState('S'))
	{
		moveCamera(XMFLOAT3(0, 0, -1), movementSpeed);
	}
	if (GetAsyncKeyState('A'))
	{
		moveCamera(XMFLOAT3(-1, 0, 0), movementSpeed);
	}
	if (GetAsyncKeyState('D'))
	{
		moveCamera(XMFLOAT3(1, 0, 0), movementSpeed);
	}

	if (GetAsyncKeyState(VK_SPACE))
	{
		moveCamera(XMFLOAT3(0, 1, 0), movementSpeed);
	}
	if (GetAsyncKeyState(VK_SHIFT))
	{
		moveCamera(XMFLOAT3(0, -1, 0), movementSpeed);
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		rotateCamera(0, (XM_PI)*lookSpeed, 0, lookSpeed);

	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		rotateCamera(0, (XM_PI)*lookSpeed * -1, 0, lookSpeed);
	}

	if (GetAsyncKeyState(VK_UP))
	{
		rotateCamera((XM_PI)*lookSpeed * -1, 0, 0, lookSpeed);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		rotateCamera((XM_PI)*lookSpeed, 0, 0, lookSpeed);
	}

	std::cout << "tedectInput" << std::endl;


}

void Camera::moveCamera(DirectX::XMFLOAT3 translationPos, float scalar)
{
	this->previousFrameCamPos = this->position;

	this->xRot;
	this->yRot;
	this->zRot;

	DirectX::XMFLOAT3 scrapVar = translationPos;

	DirectX::XMMATRIX xRotMatrix = DirectX::XMMatrixRotationX(this->xRot);
	DirectX::XMMATRIX yRotMatrix = DirectX::XMMatrixRotationY(this->yRot);
	DirectX::XMMATRIX zRotMatrix = DirectX::XMMatrixRotationZ(this->zRot);
	DirectX::XMMATRIX xyzMatrix = xRotMatrix * yRotMatrix * zRotMatrix;
	DirectX::XMVECTOR translationPosVectorFrom = DirectX::XMVectorSet(translationPos.x, translationPos.y, translationPos.z, 1);
	DirectX::XMVector3Transform(translationPosVectorFrom, xyzMatrix);
	DirectX::XMStoreFloat3(&translationPos, translationPosVectorFrom);

	this->position.x += scrapVar.x * scalar;
	this->position.y += scrapVar.y * scalar;
	this->position.z += scrapVar.z * scalar;

	//std::cout << camPos.x << " " << camPos.y << " " << camPos.z << " " << std::endl;
}

void Camera::rotateCamera(float xRot, float yRot, float zRot, float scalar)
{
}
