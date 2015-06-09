//-----------------------------------------------------------------------------
// CApplication
//
// This is a framework class for quickly creating Direct3D applications. Derive
// a new application class from this and implement your code there.
//
// Copyright (c) 2001 Andreas Jönsson
//-----------------------------------------------------------------------------
#ifndef APPLICATION_H
#define APPLICATION_H

#include "window.h"

class CApplication
{
public:
	CApplication();

	static CApplication *GetInstance();

	virtual HRESULT Initialize(HINSTANCE hInst, char *sCmdLine);
	virtual int     Run();

	void    Activate(bool bActivate);
	void    Resize(int nWidth, int nHeight);
	void    Pause(bool bPause);
	void    Render();

protected:
	static CApplication *m_pApplication;

	CWindow	   *m_pWindow;
	HINSTANCE   m_hInstance;
	bool        m_bActive;
};

#endif