#include "Camera.h"


Camera::Camera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{
	_eye = position;
	_at = at;
	_up = up;
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_nearDepth = nearDepth;
	_farDepth = farDepth;

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(/*XMVector3Normalize(*/XMLoadFloat3(&_eye), /*XMVector3Normalize(*/XMLoadFloat3(&_at), /*XMVector3Normalize(*/XMLoadFloat3(&_up)));
	// Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _windowWidth / (FLOAT)_windowHeight, 0.01f, 100.0f));
}

void Camera::Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_nearDepth = nearDepth;
	_farDepth = farDepth;

	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _windowWidth / (FLOAT)_windowHeight, 0.01f, 100.0f));
}

void Camera::Update()
{
	XMVECTOR Pos = XMLoadFloat3(&_eye);
	XMVECTOR LookDirection = XMLoadFloat3(&_at);
	XMVECTOR UP = XMLoadFloat3(&_up);

	//XMFLOAT3 test = XMFLOAT3(0.0f, 0.0f, 1.0f);

	//XMFLOAT3 test2 = XMFLOAT3(0.0f, 0.0f, -30.0f);

	//XMMATRIX mViewMat = XMMatrixLookToLH(XMLoadFloat3(&test2), XMLoadFloat3(&test), UP);


	//XMStoreFloat4x4(&_view, XMMatrixTranspose(mViewMat));


	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(/*XMVector3Normalize(*/XMLoadFloat3(&_eye), /*XMVector3Normalize(*/XMLoadFloat3(&_at), /*XMVector3Normalize(*/XMLoadFloat3(&_up)));
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _windowWidth / (FLOAT)_windowHeight, 0.01f, 100.0f));
}
//Not Working
//void Camera::Update2()
//{
//	XMStoreFloat4x4(&_view, XMMatrixLookToLH(XMLoadFloat3(&_eye), XMLoadFloat3(&_at), XMLoadFloat3(&_up)));
//	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _windowWidth / (FLOAT)_windowHeight, 0.01f, 100.0f));
//}

void Camera::SetPosition(XMFLOAT3 eye) 
{ 
	_eye = eye; 
}

void Camera::SetLookAt(XMFLOAT3 at)
{
	_at = at;
}

void Camera::SetUp(XMFLOAT3 up)
{
	_up = up;
}

XMFLOAT3 Camera::GetPosition()
{
	return _eye;
}

XMFLOAT3 Camera::GetLookAt()
{
	return _at;
}

XMFLOAT3 Camera::GetUp()
{
	return _up;
}

XMFLOAT4X4 Camera::GetView()
{
	return _view;
}

XMFLOAT4X4 Camera::GetProjection()
{
	return _projection;
}

XMFLOAT4X4 Camera::GetViewProjection()
{
	XMMATRIX _viewMatrix = XMLoadFloat4x4(&_view);
	XMMATRIX _projectionMatrix = XMLoadFloat4x4(&_projection);
	XMMATRIX _viewProj = _projectionMatrix * _viewMatrix;

	XMStoreFloat4x4(&_viewProjection, _viewProj);

	return _viewProjection;
}