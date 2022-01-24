#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>

using namespace DirectX;

class Camera
{
private:
	XMFLOAT3 _eye; 
	XMFLOAT3 _at; 
	XMFLOAT3 _up; 
	FLOAT _windowWidth; 
	FLOAT _windowHeight; 
	FLOAT _nearDepth; 
	FLOAT _farDepth;
	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;
	XMFLOAT4X4 _viewProjection;

public:
	Camera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);
	~Camera();

	void Update();
	//void Update2();

	void SetPosition(XMFLOAT3 eye);
	void SetLookAt(XMFLOAT3 at);
	void SetUp(XMFLOAT3 up);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLookAt();
	XMFLOAT3 GetUp();

	XMFLOAT4X4 GetView();
	XMFLOAT4X4 GetProjection();
	XMFLOAT4X4 GetViewProjection();

	void Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);
};

