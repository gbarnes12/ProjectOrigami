//-----------------------------------------------------------------------------
// Copyright (c) 2001 Andreas Jönsson
//-----------------------------------------------------------------------------
#include "graphics.h"
#include <assert.h>

// Initialize static member
CGraphics *CGraphics::m_pGraphics = 0;

//=============================================================================
CGraphics::CGraphics()
{
	m_pD3D          = 0;
    m_pD3DDevice    = 0;
	m_pVertexBuffer = 0;
	m_pIndexBuffer  = 0;

	m_pGraphics  = this;
}

//=============================================================================
CGraphics *CGraphics::GetInstance()
{
	return m_pGraphics;
}

//=============================================================================
HRESULT CGraphics::Initialize()
{
	// Create the D3D object, which is needed to create the D3DDevice.
	m_pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    if( m_pD3D == NULL )
		return E_FAIL;

	return S_OK;
}


//=============================================================================
HRESULT CGraphics::Uninitialize()
{
    if( m_pD3DDevice != NULL ) 
        m_pD3DDevice->Release();

    if( m_pD3D != NULL )
        m_pD3D->Release();

	return S_OK;
}

//=============================================================================
HRESULT CGraphics::CreateDevice(SDeviceParameters &Params)
{
	HRESULT hr;

	// Setup the parameters used to create the D3DDevice
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(D3DPRESENT_PARAMETERS) );

	if( Params.m_bWindowed == true )
	{
		D3DDISPLAYMODE d3ddm;
		D3DFORMAT DisplayFormat;

		d3dpp.Windowed         = TRUE;

		// Get the current desktop display mode
		if( FAILED( m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm ) ) )
			return E_FAIL;

		DisplayFormat = d3ddm.Format;
				
		if( Params.m_nAlphaBits > 0 ) 
			d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		else
			d3dpp.BackBufferFormat = DisplayFormat;

		// Check if hardware acceleration is available
		if( FAILED(m_pD3D->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
			                               DisplayFormat, d3dpp.BackBufferFormat, true)) ) 
			return E_FAIL;

		// Check if it's possible to use this backbuffer 
		if( FAILED(m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
			                                 DisplayFormat, D3DUSAGE_RENDERTARGET, 
								        	 D3DRTYPE_SURFACE, d3dpp.BackBufferFormat)) )
			return E_FAIL;

		if( Params.m_nDepthBits > 0 || Params.m_nStencilBits > 0 )
		{
			d3dpp.EnableAutoDepthStencil = TRUE;
			if( !FindDepthStencilFormat(Params, DisplayFormat,
				                        d3dpp.BackBufferFormat, 
										&(d3dpp.AutoDepthStencilFormat)) )
				return E_FAIL;
		}
	}
	else
	{
		// Enumerate display modes and search for a match
		//! TODO
	}	

	d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;

	// Create the D3DDevice
	hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
		                       Params.m_hTargetWindow,
                               D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                               &d3dpp, &m_pD3DDevice );
	if( FAILED(hr) )
        return E_FAIL;

	return S_OK;
}

//=============================================================================
bool CGraphics::FindDepthStencilFormat(SDeviceParameters &Params,
									   D3DFORMAT DisplayFormat,
									   D3DFORMAT BackBufferFormat,
									   D3DFORMAT *pDepthStencilFormat)
{
	if( Params.m_nDepthBits + Params.m_nStencilBits > 32 )
		return false;

	if( Params.m_nDepthBits <= 15 )
	{
		if( Params.m_nStencilBits <= 1 )
		{
			// Check if the format can be used
			if( CheckDepthStencilFormat(DisplayFormat, BackBufferFormat, D3DFMT_D15S1) )
			{
				*pDepthStencilFormat = D3DFMT_D15S1;
				return true;
			}
		}
	}

	if( Params.m_nDepthBits <= 16 )
	{
		if( Params.m_nStencilBits == 0 )
		{
			// Check if the format can be used
			if( CheckDepthStencilFormat(DisplayFormat, BackBufferFormat, D3DFMT_D16) )
			{
				*pDepthStencilFormat = D3DFMT_D16;
				return true;
			}
		}
	}

	if( Params.m_nDepthBits <= 24 )
	{
		if( Params.m_nStencilBits == 0 )
		{
			// Check if the format can be used
			if( CheckDepthStencilFormat(DisplayFormat, BackBufferFormat, D3DFMT_D24X8) )
			{
				*pDepthStencilFormat = D3DFMT_D24X8;
				return true;
			}
		}

		if( Params.m_nStencilBits <= 4 )
		{
			// Check if the format can be used
			if( CheckDepthStencilFormat(DisplayFormat, BackBufferFormat, D3DFMT_D24X4S4) )
			{
				*pDepthStencilFormat = D3DFMT_D24X4S4;
				return true;
			}
		}

		if( Params.m_nStencilBits <= 8 )
		{
			// Check if the format can be used
			if( CheckDepthStencilFormat(DisplayFormat, BackBufferFormat, D3DFMT_D24S8) )
			{
				*pDepthStencilFormat = D3DFMT_D24S8;
				return true;
			}
		}
	}

	if( Params.m_nDepthBits <= 32 )
	{
		if( Params.m_nStencilBits == 0 )
		{
			// Check if the format can be used
			if( CheckDepthStencilFormat(DisplayFormat, BackBufferFormat, D3DFMT_D32) )
			{
				*pDepthStencilFormat = D3DFMT_D32;
				return true;
			}
		}
	}

	return false;
}

//=============================================================================
bool CGraphics::CheckDepthStencilFormat(D3DFORMAT DisplayFormat,
										D3DFORMAT BackBufferFormat,
									    D3DFORMAT DepthStencilFormat)
{
	HRESULT hr;
	
	// Check if the format is available
	hr = m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
								   DisplayFormat, D3DUSAGE_DEPTHSTENCIL,
								   D3DRTYPE_SURFACE, DepthStencilFormat);
	if( FAILED(hr) )
		return false;

	// Verify that the depth format is compatible.
	hr = m_pD3D->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
										DisplayFormat, BackBufferFormat,
						  				DepthStencilFormat);

	if( FAILED(hr) )
		return false;

	return true;
}

//=============================================================================
IDirect3D8 *CGraphics::GetD3D()
{
	return m_pD3D;
}

//=============================================================================
IDirect3DDevice8 *CGraphics::GetD3DDevice()
{
	return m_pD3DDevice;
}

//=============================================================================
HRESULT CGraphics::CreateVertexBuffer(UINT nLength, UINT nVertexSize, 
									  DWORD FVF, D3DPOOL Pool)
{
	assert(m_pD3DDevice != 0);
	assert(m_pVertexBuffer == 0);

	HRESULT hr;

	hr = m_pD3DDevice->CreateVertexBuffer(nLength*nVertexSize,
                             D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, FVF,
                             Pool, &m_pVertexBuffer);
	if( FAILED(hr) )
		return E_FAIL;

	// Prepare for rendering with this vertex buffer
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, nVertexSize);
	m_pD3DDevice->SetVertexShader(FVF);
	
	m_nVertexSize   = nVertexSize;
	m_nUsedVertices = nLength;
	m_nMaxVertices  = nLength;

	return S_OK;
}

//=============================================================================
int CGraphics::LockVertexBuffer(UINT nVertices, BYTE **ppVertices)
{
	assert(m_pVertexBuffer != 0);
	assert(ppVertices != 0);
	assert(nVertices <= m_nMaxVertices);

	HRESULT hr;
	int nOffset;
	if(1 || m_nUsedVertices + nVertices > m_nMaxVertices )
	{
		hr = m_pVertexBuffer->Lock(0, nVertices*m_nVertexSize, ppVertices, D3DLOCK_DISCARD);
		m_nUsedVertices = nVertices;
		nOffset = 0;
	}
	else
	{
		hr = m_pVertexBuffer->Lock(m_nUsedVertices*m_nVertexSize, nVertices*m_nVertexSize, ppVertices, D3DLOCK_NOOVERWRITE);
		nOffset = m_nUsedVertices;
		m_nUsedVertices += nVertices;
	}
	if( FAILED( hr ) )
		return -1;

	return nOffset;
}

//=============================================================================
void CGraphics::UnlockVertexBuffer()
{
	assert(m_pVertexBuffer != 0);

	m_pVertexBuffer->Unlock();
}

//=============================================================================
HRESULT CGraphics::CreateIndexBuffer(UINT nLength, D3DPOOL Pool)
{
	assert(m_pD3DDevice != 0);
	assert(m_pIndexBuffer == 0);

	HRESULT hr;

	hr = m_pD3DDevice->CreateIndexBuffer(nLength*sizeof(WORD), 
		                           D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
 	                               D3DFMT_INDEX16, Pool, &m_pIndexBuffer);
	if( FAILED(hr) )
		return E_FAIL;

	// Prepare for rendering with this vertex buffer
	m_pD3DDevice->SetIndices(m_pIndexBuffer, 0);
	
	m_nIndexSize   = sizeof(WORD);
	m_nUsedIndices = nLength;
	m_nMaxIndices  = nLength;

	return S_OK;
}

//=============================================================================
int CGraphics::LockIndexBuffer(UINT nIndices, BYTE **ppIndices)
{
	assert(m_pIndexBuffer != 0);
	assert(ppIndices != 0);
	assert(nIndices <= m_nMaxIndices);

	HRESULT hr;
	int nOffset;
	if( 1||m_nUsedIndices + nIndices > m_nMaxIndices )
	{
		hr = m_pIndexBuffer->Lock(0, nIndices*m_nIndexSize, ppIndices, D3DLOCK_DISCARD);
		m_nUsedIndices = nIndices;
		nOffset = 0;
	}
	else
	{
		hr = m_pIndexBuffer->Lock(m_nUsedIndices*m_nIndexSize, nIndices*m_nIndexSize, ppIndices, D3DLOCK_NOOVERWRITE);
		nOffset = m_nUsedIndices;
		m_nUsedIndices += nIndices;
	}

	if( FAILED( hr ) )
		return -1;

	return nOffset;
}

//=============================================================================
void CGraphics::UnlockIndexBuffer()
{
	assert(m_pIndexBuffer != 0);

	m_pIndexBuffer->Unlock();
}

//=============================================================================
void CGraphics::SetIndexOffset(UINT nOffset)
{
	assert(m_pD3DDevice);
	assert(m_pIndexBuffer);

	m_pD3DDevice->SetIndices(m_pIndexBuffer, nOffset);
}