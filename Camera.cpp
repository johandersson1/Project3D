#include "Camera.h"
char debugText[100];
Camera::Camera()
{
	
}

Camera::Camera(XMFLOAT3 position, XMFLOAT3 direction, float speed)
{
	this->position = position;
	this->direction = direction;
	this->xRotdir = 0;
	this->yRotdir = 0;
	this->zRotdir = 0;
	POINT cursorData;
	GetCursorPos(&cursorData);

	setupCam();
}

Camera::~Camera()
{
}

void Camera::setupCam()
{
	this->viewMatrix = DirectX::XMMatrixLookAtLH(XMVectorSet(this->position.x, this->position.y, this->position.z, 0), XMVectorSet(this->direction.x, this->direction.y, this->direction.z, 0), XMVectorSet(0,1,0,0)); //skapar vänster-orienterat koordinatsystem,
	this->cameraPerspective = DirectX::XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)1280 / 720, 0.1f, 20.0f); //projicering matris baserad på field of view, 
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
}

void Camera::moveCamera(DirectX::XMFLOAT3 translationPos, float scalar)
{
	this->previousFrameCamPos = this->position;

	this->xRotdir;
	this->yRotdir;
	this->zRotdir;

	DirectX::XMFLOAT3 scrapVar = translationPos;

	DirectX::XMMATRIX xRotMatrix = DirectX::XMMatrixRotationX(this->xRotdir);
	DirectX::XMMATRIX yRotMatrix = DirectX::XMMatrixRotationY(this->yRotdir);
	DirectX::XMMATRIX zRotMatrix = DirectX::XMMatrixRotationZ(this->zRotdir);
	DirectX::XMMATRIX xyzMatrix = xRotMatrix * yRotMatrix * zRotMatrix;
	DirectX::XMVECTOR translationPosVectorFrom = DirectX::XMVectorSet(translationPos.x, translationPos.y, translationPos.z, 1);
	DirectX::XMVector3Transform(translationPosVectorFrom, xyzMatrix);
	DirectX::XMStoreFloat3(&translationPos, translationPosVectorFrom);

	this->position.x += scrapVar.x * scalar;
	this->position.y += scrapVar.y * scalar;
	this->position.z += scrapVar.z * scalar;
	//std::cout << "tedectInput" << std::endl;
	std::cout << position.x << " " << position.y << " " << position.z << " " << std::endl;
}

void Camera::rotateCamera(float xRot, float yRot, float zRot, float scalar)
{
	this->yRotdir += xRot * scalar;
	this->xRotdir += yRot * scalar;
	this->zRotdir += zRot * scalar;

	//sprintf_s(debugText, "My variable is %f\n", yRotdir);

	//OutputDebugStringA(debugText);

	DirectX::XMMATRIX xRotMatrix = DirectX::XMMatrixRotationX(xRot * scalar);
	DirectX::XMMATRIX yRotMatrix = DirectX::XMMatrixRotationY(yRot * scalar);
	DirectX::XMMATRIX zRotMatrix = DirectX::XMMatrixRotationZ(zRot * scalar);
	DirectX::XMMATRIX xyzMatrix = xRotMatrix * yRotMatrix * zRotMatrix;
	DirectX::XMVECTOR camDirXMVEC = DirectX::XMVectorSet(this->direction.x, this->direction.y, this->direction.z, 0);
	camDirXMVEC = DirectX::XMVector3Transform(camDirXMVEC, xyzMatrix);
	DirectX::XMStoreFloat3(&this->direction, camDirXMVEC);
}

void Camera::clean()
{
	
	if (xRotdir > 6.28318530718)
	{
		xRotdir = 0;
	}
	else if (xRotdir < 0)
	{
		xRotdir = 6.28318530718;
	}
	1.57079632679;

	if (yRotdir > 1.57079632679)
	{
		yRotdir = 1.57079632679;
	}
	else if (yRotdir < -1.57079632679)
	{
		yRotdir = -1.57079632679;
	}
	this->yRotdir = 0;
	this->zRotdir = 0;

}
