#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "DDSTextureLoader.h"
#include "Structures.h"
#include "OBJLoader.h"
#include "Camera.h"

using namespace DirectX;

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*           _pCubeVertexBuffer;
	ID3D11Buffer*           _pCubeIndexBuffer;
	ID3D11Buffer*			_pPyramidVertexBuffer;
	ID3D11Buffer*			_pPyramidIndexBuffer;
	ID3D11Buffer*           _pPlaneVertexBuffer;
	ID3D11Buffer*           _pPlaneIndexBuffer;
	ID3D11Buffer*           _pConstantBuffer;
	ID3D11ShaderResourceView* _pTextureRV = nullptr;
	ID3D11ShaderResourceView* _pTextureSun = nullptr;
	ID3D11ShaderResourceView* _pTextureEarth = nullptr;
	ID3D11ShaderResourceView* _pTextureTree = nullptr;
	ID3D11SamplerState* _pSamplerLinear = nullptr;
	XMFLOAT4X4              _world, _world2, _world3, _world4, _world5, _pyraWorld1, _sphereWorld1, _sphereWorld2, _planeWorld1, _planeWorld2, _planeWorld3, _planeWorld4;
	//XMFLOAT4X4              _view;
	//XMFLOAT4X4              _projection;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11RasterizerState* _wireFrame;
	ID3D11RasterizerState* _solidFrame;
	ID3D11BlendState* Transparency;
	XMFLOAT4 diffuseMaterial;
	XMFLOAT4 diffuseLight;
	XMFLOAT4 ambientMaterial;
	XMFLOAT4 ambientLight;
	XMFLOAT4 specularMaterial;
	XMFLOAT4 specularLight;
	float specularPower;
	XMFLOAT3 eyePosW;
	XMFLOAT3 lightDirection;
	float gTime;
	MeshData objMeshData;
	int CameraUse = 1;
	Camera* camera;
	Camera* camera2;

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();

	UINT _WindowHeight;
	UINT _WindowWidth;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();
};

