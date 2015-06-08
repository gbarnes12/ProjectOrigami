//-----------------------------------------------------------------------------
// CGraphics
//
// This is a class that takes care of most of the DirectX Graphics code
//
// Requires d3d8.lib when compiled
//
// Copyright (c) 2001 Andreas Jönsson
//-----------------------------------------------------------------------------
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <d3d8.h>

class CGraphics
{
public:
	struct SDeviceParameters
	{
		HWND m_hTargetWindow;
		bool m_bWindowed;
		UINT m_nWidth;
		UINT m_nHeight;
		UINT m_nColorBits;   // 16 or 24
		UINT m_nDepthBits;   // 0, 15, 16, 24, or 32
		UINT m_nStencilBits; // 0, 1, 4, 8
		UINT m_nAlphaBits;   // 0 or 8
	};

	CGraphics();
	static CGraphics *GetInstance();

	HRESULT Initialize();
	HRESULT CreateDevice(SDeviceParameters &Params);
	HRESULT Uninitialize();

	HRESULT CreateVertexBuffer(UINT nLength, UINT nVertexSize, DWORD FVF, D3DPOOL Pool);
	int     LockVertexBuffer(UINT nVertices, BYTE **ppVertices);
	void    UnlockVertexBuffer();

	HRESULT CreateIndexBuffer(UINT nLength, D3DPOOL Pool);
	int     LockIndexBuffer(UINT nIndices, BYTE **ppIndices);
	void    UnlockIndexBuffer();
	void    SetIndexOffset(UINT nOffset);

	IDirect3D8       *GetD3D();
	IDirect3DDevice8 *GetD3DDevice();

protected:
	bool FindDepthStencilFormat(SDeviceParameters &Params,
		                        D3DFORMAT DisplayFormat,
	                            D3DFORMAT BackBufferFormat,
	                            D3DFORMAT *pDepthStencilFormat);
	bool CheckDepthStencilFormat(D3DFORMAT DisplayFormat,
		                         D3DFORMAT BackBufferFormat, 
		                         D3DFORMAT DepthStencilFormat);

	static CGraphics *m_pGraphics;

	IDirect3D8             *m_pD3D;
	IDirect3DDevice8       *m_pD3DDevice;
	IDirect3DVertexBuffer8 *m_pVertexBuffer;
	UINT                    m_nVertexSize;
	UINT                    m_nUsedVertices;
	UINT                    m_nMaxVertices;

	IDirect3DIndexBuffer8  *m_pIndexBuffer;
	UINT                    m_nIndexSize;
	UINT                    m_nUsedIndices;
	UINT                    m_nMaxIndices;
};

#endif