#include "Graphics.h"
#include "Memtrack.h"
// TODO: cleanup text and bitmap renderers

Graphics::Graphics()
{
	m_D3D = 0;
	m_Camera = 0;
}


Graphics::Graphics(const Graphics& other)
{
}


Graphics::~Graphics()
{
}


bool Graphics::Initialize(HWND hwnd)
{
	D3DXMATRIX baseViewMatrix;
	bool result;

	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if (!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(hwnd, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new Camera;
	if (!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);
	  
	Rect::getInstance().setScreenSize(Settings::getInstance().getScreenWidth(), Settings::getInstance().getScreenHeight());
	Rect::getInstance().init(m_D3D->GetDevice(), m_D3D->GetDeviceContext());

	Line::getInstance().setScreenSize(Settings::getInstance().getScreenWidth(), Settings::getInstance().getScreenHeight());
	Line::getInstance().init(m_D3D->GetDevice(), m_D3D->GetDeviceContext());

	Text::getInstance().init(m_D3D->GetDevice(), m_D3D->GetDeviceContext());
	//doc = new Document(); 
	objectTree = new ClassTree(); 
	//objectTree->printTree(objectTree->root,0,true);
	//doc->constructDom("warlockgui");
	
	edit = new EditBox();
	
	std::vector<GUIBase *> gui;

	gui.push_back(edit);
	return true;
}


void Graphics::Shutdown()
{
	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	Rect::getInstance().cleanup();
	ID3D11Debug *pDebug;
	HRESULT hr = m_D3D->GetDevice()->QueryInterface(IID_PPV_ARGS(&pDebug));
	if (pDebug != nullptr)
	{
		pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		pDebug = nullptr;
	}

	// Release the D3D object.
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	
	//doc->cleanup();
	//if (doc)
	//{
	//	delete doc;
	//}

	

	return;
}


bool Graphics::Frame()
{
	bool result;


	// Render the graphics scene.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}
void BindRT(
	ID3D11Device *device,    // Current graphics device
	ID3D11Texture2D *ds,     // Depth-Stencil buffer to be bound
	ID3D11Texture2D *rt)     // Color buffer to be bound
{
	ID3D11RenderTargetView *rtv = NULL;
	ID3D11DepthStencilView *dsv = NULL;
	ID3D11DeviceContext *context = NULL;

	device->GetImmediateContext(&context);

	if (rt)
		if (FAILED(device->CreateRenderTargetView(rt, NULL, &rtv)))
			exit(-1);
	if (ds)
		if (FAILED(device->CreateDepthStencilView(ds, NULL, &dsv)))
			exit(-1);

	context->OMSetRenderTargets(rtv ? 1 : 0, &rtv, dsv);

	if (dsv)
		dsv->Release();
	if (rtv)
		rtv->Release();
	context->Release();
}

bool Graphics::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();
	
	objectTree->renderTree();

	/*std::vector<D3DXVECTOR2> pts;

	pts.push_back(D3DXVECTOR2(0, 0));
	pts.push_back(D3DXVECTOR2(100, 200));
	pts.push_back(D3DXVECTOR2(111, 111));
	pts.push_back(D3DXVECTOR2(1920, 1080));
	pts.push_back(D3DXVECTOR2(700, 300));
	pts.push_back(D3DXVECTOR2(800, 600));
	pts.push_back(D3DXVECTOR2(80, 30));
	pts.push_back(D3DXVECTOR2(565, 300));
	pts.push_back(D3DXVECTOR2(82, 780));
	pts.push_back(D3DXVECTOR2(145, 550));
	pts.push_back(D3DXVECTOR2(788, 300));*/

	//Line::getInstance().catmullRomSpline(pts, 10);

	Rect::getInstance().SetShaderParameters(m_D3D->GetDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
	Rect::getInstance().flushToDrawBuffer(m_D3D->GetDevice(), m_D3D->GetDeviceContext());

	//Line::getInstance().SetShaderParameters(m_D3D->GetDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
	//Line::getInstance().flushToDrawBuffer(m_D3D->GetDevice(), m_D3D->GetDeviceContext());
	Text::getInstance().updateDevCon(m_D3D->GetDeviceContext());
	Text::getInstance().drawSyntaxHighlightedText(m_D3D->GetDevice(), 500.0f, 500.0f, 1.0f, edit->getText(), FW1_CENTER, edit->getCursorPos());
	Text::getInstance().drawText();
	
	//doc->renderButtonText(m_D3D->GetDeviceContext());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_D3D->EndScene();
	

	//doc->walker.refresh = false;
	return true;
}