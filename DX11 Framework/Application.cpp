#include "Application.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pCubeVertexBuffer = nullptr;
	_pCubeIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	// Initialize the world matrix
	XMStoreFloat4x4(&_world, XMMatrixIdentity());

 //   // Initialize the view matrix
	//XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f);
	//XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	//XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));

 //   // Initialize the projection matrix
	//XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _WindowWidth / (FLOAT) _WindowHeight, 0.01f, 100.0f));

    //camera 1
    XMFLOAT3 Eye = XMFLOAT3(0.0f, 0.0f, -3.0f);
    XMFLOAT3 At = XMFLOAT3(0.0f, 0.0f, 0.1f);
    XMFLOAT3 Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
    camera = new Camera(Eye, At, Up, _WindowWidth, _WindowHeight, 0.01f, 100.0f);

    //camera 2
    Eye = XMFLOAT3(0.0f, 5.0f, 0.0f);
    At = XMFLOAT3(0.0f, 0.0f, 0.0f);
    Up = XMFLOAT3(0.0f, 0.0f, 1.0f);
    camera2 = new Camera(Eye, At, Up, _WindowWidth, _WindowHeight, 0.01f, 100.0f);

    objMeshData = OBJLoader::Load("sphere.obj", _pd3dDevice);

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;
    //CUBE SETUP
    // Create vertex buffer

    //XMFLOAT3 cv0 = XMFLOAT3(1.0f, 1.0f, -1.0f);
    //XMFLOAT3 cv1 = XMFLOAT3(-1.0f, 1.0f, -1.0f);
    //XMFLOAT3 cv2 = XMFLOAT3(-1.0f, 1.0f, 1.0f);
    //XMFLOAT3 cv3 = XMFLOAT3(1.0f, 1.0f, 1.0f);

    //XMFLOAT3 cv4 = XMFLOAT3(1.0f, -1.0f, -1.0f);
    //XMFLOAT3 cv5 = XMFLOAT3(-1.0f, -1.0f, -1.0f);
    //XMFLOAT3 cv6 = XMFLOAT3(-1.0f, -1.0f, 1.0f);
    //XMFLOAT3 cv7 = XMFLOAT3(1.0f, -1.0f, 1.0f);

    SimpleVertex vertices[] =
    {   //Vertices                     Normals                                      TexCoords
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.333333f, 0.666667f, -0.666667f) , XMFLOAT2(1.0f, 0.0f)},//0
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-0.816497f, 0.408248f, -0.408248f) , XMFLOAT2(0.0f, 0.0f)},//1
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-0.333333f, 0.666667f, 0.666667f) , XMFLOAT2(1.0f, 0.0f)},//2
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.816497f, 0.408248f, 0.408248f) , XMFLOAT2(0.0f, 0.0f)},//3
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.666667f, -0.666667f, -0.333333f) , XMFLOAT2(1.0f, 1.0f)},//4
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-0.408248f, -0.408248f, -0.816497f) , XMFLOAT2(0.0f, 1.0f)},//5
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-0.666667f, -0.666667f, 0.333333f) , XMFLOAT2(1.0f, 1.0f)},//6
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.408248f, -0.408248f, 0.816497f) , XMFLOAT2(0.0f, 1.0f)},//7
    };

    D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    //altered
    bd.ByteWidth = sizeof(SimpleVertex) * 8;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;

    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pCubeVertexBuffer);

    //PYRAMID SETUP
    //Vertices
    //XMFLOAT3 pv0 = XMFLOAT3(0.0, 1.0, 0.0);
    //XMFLOAT3 pv1 = XMFLOAT3(-1.0, -1.0, -1.0);
    //XMFLOAT3 pv2 = XMFLOAT3(1.0, -1.0, -1.0);
    //XMFLOAT3 pv3 = XMFLOAT3(-1.0, -1.0, 1.0);
    //XMFLOAT3 pv4 = XMFLOAT3(1.0, -1.0, 1.0);

    // Create vertex buffer
    SimpleVertex pyramidvertices[] =
    {   //Vertices                 Normals                     TexCoords
        { XMFLOAT3(0.0, 1.0, 0.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0, -1.0, -1.0), XMFLOAT3(-0.298157f, -0.149071f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0, -1.0, -1.0), XMFLOAT3(0.298157f, -0.149071f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0, -1.0, 1.0), XMFLOAT3(-0.298157f, 0.035191f, 0.298157f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0, -1.0, 1.0), XMFLOAT3(-0.298157f, 0.035191f, 0.298157f), XMFLOAT2(1.0f, 1.0f) },
    };


    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    //altered
    bd.ByteWidth = sizeof(SimpleVertex) * 5;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = pyramidvertices;

    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPyramidVertexBuffer);


    // Create plane vertex buffer
    SimpleVertex planevertices[] =
    {   //Vertices                 Normals                     TexCoords
        { XMFLOAT3(2.0, -2.0, -2.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },//1
        { XMFLOAT3(1.0, -2.0, -2.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.75f, 1.0f) },//2
        { XMFLOAT3(0.0, -2.0, -2.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.5f, 1.0f) },//3
        { XMFLOAT3(-1.0, -2.0, -2.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.25f, 1.0f) },//4
        { XMFLOAT3(-2.0, -2.0, -2.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },//5

        { XMFLOAT3(2.0, -2.0, -1.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.75f) },//6
        { XMFLOAT3(1.0, -2.0, -1.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.75f, 0.75f) },//7
        { XMFLOAT3(0.0, -2.0, -1.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.5f, 0.75f) },//8
        { XMFLOAT3(-1.0, -2.0, -1.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.25f, 0.75f) },//9
        { XMFLOAT3(-2.0, -2.0, -1.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.75f) },//10

        { XMFLOAT3(2.0, -2.0, 0.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.5f) },//11
        { XMFLOAT3(1.0, -2.0, 0.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.75f, 0.5f) },//12
        { XMFLOAT3(0.0, -2.0, 0.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.5f, 0.5f) },//13
        { XMFLOAT3(-1.0, -2.0, 0.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.25f, 0.5f) },//14
        { XMFLOAT3(-2.0, -2.0, 0.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.5f) },//15

        { XMFLOAT3(2.0, -2.0, 1.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.25f) },//16
        { XMFLOAT3(1.0, -2.0, 1.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.75f, 0.25f) },//17
        { XMFLOAT3(0.0, -2.0, 1.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.5f, 0.25f) },//18
        { XMFLOAT3(-1.0, -2.0, 1.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.25f, 0.25f) },//19
        { XMFLOAT3(-2.0, -2.0, 1.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.25f) },//20

        { XMFLOAT3(2.0, -2.0, 2.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },//21
        { XMFLOAT3(1.0, -2.0, 2.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.75f, 0.0f) },//22
        { XMFLOAT3(0.0, -2.0, 2.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.5f, 0.0f) },//23
        { XMFLOAT3(-1.0, -2.0, 2.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.25f, 0.0f) },//24
        { XMFLOAT3(-2.0, -2.0, 2.0), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },//25
    };


    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    //altered
    bd.ByteWidth = sizeof(SimpleVertex) * 25;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = planevertices;

    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPlaneVertexBuffer);

    if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

    // Create index buffer
    //altered
    WORD indices[] =
    {
        0,1,2,
        0,2,3,
        0,4,5,
        0,5,1,
        1,5,6,
        1,6,2,
        2,6,7,
        2,7,3,
        3,7,4,
        3,4,0,
        4,7,6,
        4,6,5
    };

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;     
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indices;
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pCubeIndexBuffer);

    // PYRAMID SETUP
    WORD pyramidindices[] =
    {
        0,2,1,
        0,1,3,
        0,4,2,
        0,3,4,
        1,2,3,
        3,2,4
    };

    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 18;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = pyramidindices;
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPyramidIndexBuffer);

    // PLANE SETUP
    WORD planeindices[] =
    {
        0,1,5,
        1,6,5,
        1,2,6,
        2,7,6,
        2,3,7,
        3,8,7,
        3,4,8,
        4,9,8,
        9,14,13,
        8,9,13,
        8,13,12,
        7,8,12,
        7,12,11,
        6,7,11,
        6,11,10,
        5,6,10,
        10,11,15,
        11,16,15,
        11,12,16,
        12,17,16,
        12,13,17,
        13,18,17,
        13,14,18,
        14,19,18,
        19,24,23,
        18,19,23,
        18,23,22,
        17,18,22,
        17,22,21,
        16,17,21,
        16,21,20,
        15,16,20
    };

    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 96;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = planeindices;
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPlaneIndexBuffer);

    if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 640, 480};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

    //Create depth stencil buffer
    D3D11_TEXTURE2D_DESC depthStencilDesc;

    depthStencilDesc.Width = _WindowWidth;
    depthStencilDesc.Height = _WindowHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    _pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
    _pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView); 

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

	InitVertexBuffer();

    // Set vertex buffer
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    _pImmediateContext->IASetVertexBuffers(0, 1, &_pCubeVertexBuffer, &stride, &offset);
    

	InitIndexBuffer();

    // Set index buffer
    _pImmediateContext->IASetIndexBuffer(_pCubeIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

    //Create WireFrame Rasterizer
    D3D11_RASTERIZER_DESC wfdesc;
    ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
    wfdesc.FillMode = D3D11_FILL_WIREFRAME;
    wfdesc.CullMode = D3D11_CULL_NONE;
    hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);
    _pImmediateContext->RSSetState(_wireFrame);

    //Create Solid Rasterizer
    D3D11_RASTERIZER_DESC sddesc;
    ZeroMemory(&sddesc, sizeof(D3D11_RASTERIZER_DESC));
    sddesc.FillMode = D3D11_FILL_SOLID;
    sddesc.CullMode = D3D11_CULL_NONE;
    hr = _pd3dDevice->CreateRasterizerState(&sddesc, &_solidFrame);
    _pImmediateContext->RSSetState(_solidFrame);

    //Load Texture
    CreateDDSTextureFromFile(_pd3dDevice, L"Images/Crate_COLOR.dds", nullptr, &_pTextureRV);
    CreateDDSTextureFromFile(_pd3dDevice, L"Images/Sun.dds", nullptr, &_pTextureSun);
    CreateDDSTextureFromFile(_pd3dDevice, L"Images/PineTree.dds", nullptr, &_pTextureTree);
    CreateDDSTextureFromFile(_pd3dDevice, L"Images/Earth.dds", nullptr, &_pTextureEarth);
    

    // Create the sample state
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    _pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);
    _pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));

    D3D11_RENDER_TARGET_BLEND_DESC rtbd;
    ZeroMemory(&rtbd, sizeof(rtbd));
    rtbd.BlendEnable = true;
    rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
    rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
    rtbd.BlendOp = D3D11_BLEND_OP_ADD;
    rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
    rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
    rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.RenderTarget[0] = rtbd;
    _pd3dDevice->CreateBlendState(&blendDesc, &Transparency);

    if (FAILED(hr))
        return hr;

    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();

    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pCubeVertexBuffer) _pCubeVertexBuffer->Release();
    if (_pCubeIndexBuffer) _pCubeIndexBuffer->Release();
    if (_pPyramidVertexBuffer) _pPyramidVertexBuffer->Release();
    if (_pPyramidIndexBuffer) _pPyramidIndexBuffer->Release();
    if (_pPlaneVertexBuffer) _pPlaneVertexBuffer->Release();
    if (_pPlaneIndexBuffer) _pPlaneIndexBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
    if (_depthStencilView) _depthStencilView->Release();
    if (_depthStencilBuffer) _depthStencilBuffer->Release();
    if (_wireFrame) _wireFrame->Release();
    if (_solidFrame) _solidFrame->Release();
    if (Transparency) Transparency->Release();
}

void Application::Update()
{
    // Update our time
    float t = 0.0f;

    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float) XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();

        if (dwTimeStart == 0)
            dwTimeStart = dwTimeCur;

        t = (dwTimeCur - dwTimeStart) / 1000.0f;
    }

    gTime = t;


    // Light direction from surface (XYZ)
    lightDirection = XMFLOAT3(0.25f, 0.5f, -1.0f);
    // Diffuse material properties (RGBA)
    diffuseMaterial = XMFLOAT4(0.8f, 0.5f, 0.5f, 1.0f);
    // Diffuse light colour (RGBA)
    diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    // Ambient material properties (RGBA)
    ambientMaterial = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
    // Ambient light colour (RGBA)
    ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
    // Specular material properties (RGBA)
    specularMaterial = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    // Specular light colour (RGBA)
    specularLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    // Specular Power
    specularPower = 10.0f;
    //Eye Position
    eyePosW = XMFLOAT3(0.0f, 0.0f, -3.0f);

    //Camera Change + Mechanics
    if (GetAsyncKeyState('1'))
    {
        CameraUse = 1;
        camera->SetPosition(XMFLOAT3(0.0f, 0.0f, -3.0f));
    }
    else if (GetAsyncKeyState('2'))
    {
        CameraUse = 2;
        camera2->SetPosition(XMFLOAT3(0.0f, 5.0f, 0.0f));
    }

    if (CameraUse == 1)
    {

        XMFLOAT3 temp = XMFLOAT3(0.0f, 0.0f, 0.0f);

        if (GetAsyncKeyState('W'))
        {
            temp.z += 0.002;
        }
        else if (GetAsyncKeyState('S'))
        {
            temp.z -= 0.002;
        }
        else if (GetAsyncKeyState('A'))
        {
            temp.x -= 0.002;
        }
        else if (GetAsyncKeyState('D'))
        {
            temp.x += 0.002;
        }

        camera->SetPosition(XMFLOAT3((camera->GetPosition().x + temp.x), (camera->GetPosition().y + temp.y), (camera->GetPosition().z + temp.z)));
    }
    else if (CameraUse == 2)
    {
        XMFLOAT3 temp = XMFLOAT3(0.0f, 0.0f, 0.0f);

        if (GetAsyncKeyState('W'))
        {
            temp.z += 0.002;
        }
        else if (GetAsyncKeyState('S'))
        {
            temp.z -= 0.002;
        }
        else if (GetAsyncKeyState('A'))
        {
            temp.x -= 0.002;
        }
        else if (GetAsyncKeyState('D'))
        {
            temp.x += 0.002;
        }

        camera2->SetPosition(XMFLOAT3((camera2->GetPosition().x + temp.x), (camera2->GetPosition().y + temp.y), (camera2->GetPosition().z + temp.z)));
    }

    camera->Update();
    camera2->Update();

    // Animate the cube
    //LARGE CUBE
	XMStoreFloat4x4(&_world, XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(t));
    //PLANETS
    XMStoreFloat4x4(&_world2, XMMatrixTranslation(10.0f, 0.0f, 1.0f) * XMMatrixScaling(0.25f, 0.25f, 0.25f) * XMMatrixRotationY(-t));
    XMStoreFloat4x4(&_world3, XMMatrixTranslation(-10.0f, 0.0f, 1.0f) * XMMatrixScaling(0.25f, 0.25f, 0.25f) * XMMatrixRotationY(-t));
    //MOONS
    XMStoreFloat4x4(&_world4, XMMatrixTranslation(15.0f, 0.0f, 1.0f) * XMMatrixScaling(0.25f, 0.25f, 0.25f) * XMMatrixRotationY(t * 5) * XMLoadFloat4x4(&_world2));
    XMStoreFloat4x4(&_world5, XMMatrixTranslation(-15.0f, 0.0f, 1.0f) * XMMatrixScaling(0.25f, 0.25f, 0.25f) * XMMatrixRotationY(-t * 2) * XMLoadFloat4x4(&_world3));

    //PLANES
    XMStoreFloat4x4(&_planeWorld1, XMMatrixScaling(1.0f, 1.0f, 1.0f));
    XMStoreFloat4x4(&_planeWorld2, XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixRotationX(-1.571) * XMMatrixTranslation(0.0f, 0.0f, 2.0f));
    XMStoreFloat4x4(&_planeWorld3, XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixRotationX(-1.571) * XMMatrixTranslation(4.0f, 0.0f, 1.0f));
    XMStoreFloat4x4(&_planeWorld4, XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixRotationX(-1.571) * XMMatrixTranslation(-4.0f, 0.0f, 1.0f));

    //PYRAMIDS
    XMStoreFloat4x4(&_pyraWorld1, XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(t));

    //SPHERES
    XMStoreFloat4x4(&_sphereWorld1, XMMatrixTranslation(0.0f, 2.0f, 0.0f) * XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(t));
    XMStoreFloat4x4(&_sphereWorld2, XMMatrixTranslation(5.0f, 4.0f, 1.0f) * XMMatrixScaling(0.25f, 0.25f, 0.25f) * XMMatrixRotationY(-t));


    if (GetAsyncKeyState(VK_RBUTTON))
    {
        _pImmediateContext->RSSetState(_wireFrame);
    }
    if (GetAsyncKeyState(VK_LBUTTON))
    {
        _pImmediateContext->RSSetState(_solidFrame);
    }
}

void Application::Draw()
{
    //
    // Clear the back buffer
    //
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);

	XMMATRIX world = XMLoadFloat4x4(&_world);
	XMMATRIX view = XMLoadFloat4x4(&camera->GetView());
	XMMATRIX projection = XMLoadFloat4x4(&camera->GetProjection());

    if (CameraUse == 1)
    {
        view = XMLoadFloat4x4(&camera->GetView());
        projection = XMLoadFloat4x4(&camera->GetProjection());
    }
    else if (CameraUse == 2)
    {
        view = XMLoadFloat4x4(&camera2->GetView());
        projection = XMLoadFloat4x4(&camera2->GetProjection());
    }


    //
    // Update variables
    //
    ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(world);
	cb.mView = XMMatrixTranspose(view);
	cb.mProjection = XMMatrixTranspose(projection);
    cb.DiffuseMtrl = diffuseMaterial;
    cb.DiffuseLight = diffuseLight;
    cb.AmbientMaterial = ambientMaterial;
    cb.AmbientLight = ambientLight;
    cb.SpecularMtrl = specularMaterial;
    cb.SpecularLight = specularLight;
    cb.SpecularPower = specularPower;
    cb.LightVecW = lightDirection;
    cb.gTime = gTime;

	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    // Specify factor for the blending equation
    float blendFactor[] = { 0.75f, 0.75f, 0.75f, 1.0f };
    // Set the default blend state (no blending) for opaque objects
    _pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);

    //
    // Renders a triangle
    //
	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
    _pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
    _pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    _pImmediateContext->IASetVertexBuffers(0, 1, &_pCubeVertexBuffer, &stride, &offset);
    _pImmediateContext->IASetIndexBuffer(_pCubeIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    //draw Cube 1
	//_pImmediateContext->DrawIndexed(36, 0, 0);

    _pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV);
    

    //Cube 2
    world = XMLoadFloat4x4(&_world2);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    _pImmediateContext->DrawIndexed(36, 0, 0);

    //Cube 3
    world = XMLoadFloat4x4(&_world3);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    _pImmediateContext->DrawIndexed(36, 0, 0);

    //Cube 4
    world = XMLoadFloat4x4(&_world4);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    _pImmediateContext->DrawIndexed(36, 0, 0);

    //Cube 5
    world = XMLoadFloat4x4(&_world5);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    _pImmediateContext->DrawIndexed(36, 0, 0);

    _pImmediateContext->IASetVertexBuffers(0, 1, &_pPlaneVertexBuffer, &stride, &offset);
    _pImmediateContext->IASetIndexBuffer(_pPlaneIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    //Plane 1
    world = XMLoadFloat4x4(&_planeWorld1);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    _pImmediateContext->DrawIndexed(96, 0, 0);

    _pImmediateContext->PSSetShaderResources(0, 1, &_pTextureTree);
    //Plane 2
    world = XMLoadFloat4x4(&_planeWorld2);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    _pImmediateContext->DrawIndexed(96, 0, 0);

    //Plane 3
    world = XMLoadFloat4x4(&_planeWorld3);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    _pImmediateContext->DrawIndexed(96, 0, 0);

    //Plane 4
    world = XMLoadFloat4x4(&_planeWorld4);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    _pImmediateContext->DrawIndexed(96, 0, 0);

    // Set the blend state for transparent objects
    _pImmediateContext->OMSetBlendState(Transparency, blendFactor, 0xffffffff);

    _pImmediateContext->IASetVertexBuffers(0, 1, &_pPyramidVertexBuffer, &stride, &offset);
    _pImmediateContext->IASetIndexBuffer(_pPyramidIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    _pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV);
    //Pyramid 1
    world = XMLoadFloat4x4(&_pyraWorld1);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    _pImmediateContext->DrawIndexed(18, 0, 0);
    
    _pImmediateContext->PSSetShaderResources(0, 1, &_pTextureSun);
    _pImmediateContext->IASetVertexBuffers(0, 1, &objMeshData.VertexBuffer, &objMeshData.VBStride, &objMeshData.VBOffset);
    _pImmediateContext->IASetIndexBuffer(objMeshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    //Sphere1
    world = XMLoadFloat4x4(&_sphereWorld1);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    _pImmediateContext->DrawIndexed(objMeshData.IndexCount, 0, 0);

    _pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);
    _pImmediateContext->PSSetShaderResources(0, 1, &_pTextureEarth);
    //Sphere2
    world = XMLoadFloat4x4(&_sphereWorld2);
    cb.mWorld = XMMatrixTranspose(world);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    _pImmediateContext->DrawIndexed(objMeshData.IndexCount, 0, 0);

    //
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);
}
