//-----------------------------------------------------------------------------
// Copyright (c) 2001 Andreas Jönsson
//-----------------------------------------------------------------------------
#include "application.h"

// Initialize static member
CApplication *CApplication::m_pApplication = 0;

//-----------------------------------------------------------------------------
// CApplication()
//-----------------------------------------------------------------------------
CApplication::CApplication()
{
	m_hInstance    = 0;
    m_pWindow      = 0;
	m_bActive	   = false;

	m_pApplication = this;
}

//-----------------------------------------------------------------------------
// GetInstance()
//-----------------------------------------------------------------------------
CApplication *CApplication::GetInstance()
{
	return m_pApplication;
}


//-----------------------------------------------------------------------------
// Initialize()
//-----------------------------------------------------------------------------
HRESULT CApplication::Initialize(HINSTANCE hInst, char *sCmdLine)
{
	m_hInstance = hInst;

	return S_OK;
}


//-----------------------------------------------------------------------------
// Run()
// Message-processing loop. 
//-----------------------------------------------------------------------------
int CApplication::Run()
{
    // Now we're ready to recieve and process Windows messages.
	while( !m_pWindow->CheckMessage(!m_bActive) )
	{
	}

    return 0;
}


//-----------------------------------------------------------------------------
// Activate()
//-----------------------------------------------------------------------------
void CApplication::Activate(bool bActivate)
{
	if( bActivate )
	{
		Pause(false);

		// Restore lost surfaces
	}
	else
	{
		Pause(true);
	}
}


//-----------------------------------------------------------------------------
// Pause()
//-----------------------------------------------------------------------------
void CApplication::Pause(bool bPause)
{
	if( bPause )
	{
		m_bActive = false;
	}
	else
	{
		m_bActive = true;
	}
}


//-----------------------------------------------------------------------------
// Resize()
//-----------------------------------------------------------------------------
void CApplication::Resize(int nWidth, int nHeight)
{
	// Resize the framebuffer
}

//-----------------------------------------------------------------------------
// Render()
//-----------------------------------------------------------------------------
void CApplication::Render()
{
	// Render the last frame to the window
}

