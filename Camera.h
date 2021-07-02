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
public:
	Camera();
	Camera(XMFLOAT3 position, XMFLOAT3 direction);
	~Camera();
	void setupCam();
	XMMATRIX cameraPerspective;
	XMMATRIX cameraProjection;
	XMFLOAT3 getCameraPos() {return this->position; };
	XMFLOAT3 getCameraDir() {return this->direction; };
};