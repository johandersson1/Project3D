#include "Camera.h"
char debugText[100];

Camera::Camera()
	:pitch(0), yaw(0), rotationSpeed(0), speed(0)
{
	this->up = { 0,1,0 }; // Y
	this->forward = { 0,0,1 }; // Z
	this->right = { 1,0,0 }; 
	// Builds a view matrix for a left-handed coordinate system using a camera position, an up direction, and a focal point, XMLoadFloat3 = Loads an XMFLOAT3 into an XMVECTOR
	// kanske ändra namnet (forward)? check this!
	this->viewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&position), forward, up); 
	// Builds a left-handed perspective projection matrix based on a field of view, XM_PIDIV4 =  π/4
	perspectiveMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)1024 / float(576), 0.1f, 100.0f);

	this->direction = forward;
	this->pickingDistance = 10.0f;
}

Camera::Camera(float FOV, float aspectRatio, float nearZ, float farZ, float rotationSpeed, float speed, XMFLOAT3 position)
	:position(position), pitch(0), yaw(-XM_PIDIV2), rotationSpeed(rotationSpeed), speed(speed)
{

	this->up = { 0,1,0 };
	this->forward = { 0, 0, 1 };
	this->right = { 1,0,0 };
	this->viewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&position), forward, up);

	this->perspectiveMatrix = XMMatrixPerspectiveFovLH(FOV, aspectRatio, nearZ, farZ);

	this->direction = forward;

	this->FOV = FOV;
	this->aspectRatio = aspectRatio;
	this->nearZ = nearZ;
	this->farZ = farZ;

}

void Camera::MoveRight(float dt)
{
	XMVECTOR forwardVec = XMVector3Transform(forward,
		XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) *
		XMMatrixScaling(speed, speed, speed));

	XMVECTOR rightVec = XMVector3Cross(up, forwardVec);
	rightVec = XMVector3Normalize(rightVec);
	XMFLOAT3 rightFloat;
	XMStoreFloat3(&rightFloat, rightVec);

	position.x += rightFloat.x * dt * speed;
	position.y += rightFloat.y * dt * speed;
	position.z += rightFloat.z * dt * speed;
}

void Camera::MoveForward(float dt)
{
	XMVECTOR forwardVec = XMVector3Transform(forward,
		XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) *
		XMMatrixScaling(speed, speed, speed));

	forwardVec = XMVector3Normalize(forwardVec);
	XMFLOAT3 forwardFloat;

	XMStoreFloat3(&forwardFloat, forwardVec);

	position.x += forwardFloat.x * dt * speed;
	position.y += forwardFloat.y * dt * speed;
	position.z += forwardFloat.z * dt * speed;
}

void Camera::MoveUpDown(float dt)
{
	XMVECTOR upDownVec = XMVector3Transform(moveUp,
		XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) *
		XMMatrixScaling(speed, speed, speed));

	upDownVec = XMVector3Normalize(upDownVec);
	XMFLOAT3 upDownFloat;

	XMStoreFloat3(&upDownFloat, upDownVec);

	//position.x += upDownFloat.x * dt * speed;
	position.y += upDownFloat.y * dt * speed;
	//position.z += upDownFloat.z * dt * speed;
}

void Camera::Rotate(float dx, float dy)
{
	yaw = (yaw + dx * rotationSpeed);
	yaw = (float)fmod((double)yaw + XM_PI, XM_2PI);
	if (yaw < 0)
		yaw += XM_2PI;
	yaw -= XM_PI;

	pitch = std::clamp(pitch + dy * rotationSpeed, 0.995f * -XM_PIDIV2, 0.995f * XM_PIDIV2);
}

void Camera::Update(float dt)
{
	// Move and change the camera on key press

	if (GetAsyncKeyState('W'))
	{
		MoveForward(dt);
		
	}
	if (GetAsyncKeyState('S'))
	{
		MoveForward(-dt);
	}
	if (GetAsyncKeyState('A'))
	{
		MoveRight(-dt);
	}
	if (GetAsyncKeyState('D'))
	{
		MoveRight(dt);
	}
	
	if (GetAsyncKeyState(VK_SHIFT))
	{
		MoveUpDown(-dt);
	}

	if (GetAsyncKeyState(VK_SPACE))
	{
		MoveUpDown(dt);
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		Rotate(dt,0);
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		Rotate(-dt, 0);
	}
	
	if (GetAsyncKeyState(VK_UP))
	{
		Rotate(0, -dt);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		Rotate(0, dt);
	}

	const XMVECTOR lookAtVec = XMVector3Transform(forward, XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f));
	const XMVECTOR target = XMLoadFloat3(&position) + lookAtVec;
	direction = XMVector3Normalize(lookAtVec);

	this->viewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&position), target, up);
}