#pragma once

#include <DirectXMath.h>
#include <Windows.h>
#include <iostream>
#include "stb_image.h"

using namespace DirectX;

class Camera
{
private:
	XMFLOAT3 position;
	XMFLOAT3 direction;;
	XMFLOAT3 lookAt;
	XMFLOAT3 upVector;
	XMFLOAT3 previousFrameCamPos;
	
	float xRotdir;
	float yRotdir;
	float zRotdir;

	XMMATRIX camRotationMatrix;
	XMVECTOR lookAtVector;
	XMVECTOR defaultForward;
	XMVECTOR defaultRight;
	XMVECTOR camRight;
	XMVECTOR camForward; 
	float moveBackForward;
	float moveLeftRight;
	float speed;
	float camPitch;
	float camYaw;


	
public:
	Camera();
	Camera(XMFLOAT3 position, XMFLOAT3 direction, float speed);
	~Camera();
	void setupCam();
	void detectInput(float movementSpeed, float lookSpeed);
	void moveCamera(DirectX::XMFLOAT3 translationPos, float scalar);
	void rotateCamera(float xRot, float yRot, float zRot, float scalar);
	void clean();
	XMMATRIX cameraPerspective;
	XMMATRIX cameraProjection;
	XMFLOAT3 getCameraPos() {return this->position; };
	XMFLOAT3 getCameraDir() {return this->direction; };
};