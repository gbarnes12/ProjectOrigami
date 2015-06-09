#include "app.h"
#include "graphics.h"
#include "marchingcubes.h"
#include <d3dx8.h>

//=============================================================================
CApp::CApp() : CApplication()
{
}

//=============================================================================
HRESULT CApp::Initialize(HINSTANCE hInst, char *sCmdLine)
{
	HRESULT hr;

	CApplication::Initialize(hInst, sCmdLine);

	// Create the window
	CWindow::SParameters Params;

	m_pWindow = new CWindow();

	m_pWindow->SetDefault(&Params);
	Params.m_hInstance = m_hInstance;
	Params.m_sTitle = "Metaballs from AngelCode.com";

	hr = m_pWindow->Initialize(Params);
	if( FAILED(hr) )
		return hr;

	//-------------------------------------------------------------------------
	m_pMetaballs = new CMetaballs();
	m_pMetaballs->SetGridSize(48);

	CMarchingCubes::BuildTables();

	m_pTimer = new CTimer();

	//-------------------------------------------------------------------------
	// Setup Direct3D

	CGraphics *pGfx = new CGraphics();
	pGfx->Initialize();

	CGraphics::SDeviceParameters GfxParams;
	ZeroMemory(&GfxParams, sizeof(GfxParams));
	GfxParams.m_bWindowed = true;
	GfxParams.m_hTargetWindow = m_pWindow->GetHandle();
	GfxParams.m_nColorBits = 16;
	GfxParams.m_nDepthBits = 16;

	if( FAILED(pGfx->CreateDevice(GfxParams)) )
	{
		GfxParams.m_nColorBits = 24;
		GfxParams.m_nDepthBits = 32;
		if( FAILED(pGfx->CreateDevice(GfxParams)) )
			return E_FAIL;
	}

	IDirect3DDevice8 *pDev = pGfx->GetD3DDevice();

	//-------------------------------------------------------------------------
	// Create buffers
	hr = pGfx->CreateVertexBuffer(MAX_VERTICES, sizeof(SVertex), 
		                          FVF_VERTEX, D3DPOOL_DEFAULT);
	if( FAILED(hr) )
		return E_FAIL;

	hr = pGfx->CreateIndexBuffer(MAX_INDICES, D3DPOOL_DEFAULT);
	if( FAILED(hr) )
		return E_FAIL;

	//-------------------------------------------------------------------------
	// Initialize renderstates

	// Set the transform matrices
	D3DXMATRIX Mtx;
	D3DXMatrixPerspectiveLH(&Mtx, 0.3f, 0.3f*3/4, 0.25f, 10);
	pDev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&Mtx);
	
	D3DXMatrixTranslation(&Mtx, 0,0,2.0f);
	pDev->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&Mtx);

	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// Ligthing
	D3DLIGHT8 Light;
	Light.Type         = D3DLIGHT_DIRECTIONAL;
	Light.Diffuse      = D3DXCOLOR(1,1,1,1);
	Light.Specular     = D3DXCOLOR(1,1,1,1);
	Light.Ambient      = D3DXCOLOR(0,0,0,0);
	Light.Direction    = D3DXVECTOR3(0,0,1);
	pDev->SetLight(0, &Light);

	// Material
	D3DMATERIAL8 Mat;
	Mat.Diffuse  = D3DXCOLOR(0.7f,0.7f,0.9f,1);
	Mat.Specular = D3DXCOLOR(1,1,1,1);
	Mat.Ambient  = D3DXCOLOR(0,0,0,1);
	Mat.Emissive = D3DXCOLOR(0,0,0,1);
	Mat.Power = 25.0f;
	pDev->SetMaterial(&Mat);

	// Enable ligthing 
	pDev->LightEnable(0, TRUE);
	pDev->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

	// Setup the lighting engine to fetch material properties from the material
	pDev->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE , D3DMCS_MATERIAL);
	pDev->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
	pDev->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE , D3DMCS_MATERIAL);
	pDev->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

	// Setup the texture for spheremapping
	hr = D3DXCreateTextureFromFile(pDev, "Spheremap.jpg", &m_pTexture);
	if( SUCCEEDED(hr) )
		pDev->SetTexture(0, m_pTexture);

	pDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	pDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	pDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	pDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_ADD);
	pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);


	return S_OK;
}

//=============================================================================
int CApp::Run()
{
	m_pTimer->Start();

    // Now we're ready to recieve and process Windows messages.
	while( !m_pWindow->CheckMessage(!m_bActive) )
	{
		// Update the metaballs' positions
		m_pMetaballs->Update(m_pTimer->GetDeltaTime());

		CGraphics *pGfx = CGraphics::GetInstance();
		IDirect3DDevice8 *pDev = pGfx->GetD3DDevice();

		pDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
			           D3DCOLOR_XRGB(24,24,24), 1.0f, 0);

		pDev->BeginScene();

		// Render the metaballs
		m_pMetaballs->Render();

		pDev->EndScene();
		pDev->Present( NULL, NULL, NULL, NULL );

		m_pTimer->Frame();
	}

	return 0;
}