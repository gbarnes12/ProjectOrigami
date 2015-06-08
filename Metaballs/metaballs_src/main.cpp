//-----------------------------------------------------------------------------
// Copyright (c) 2000 Andreas Jönsson
//-----------------------------------------------------------------------------
#include "app.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd)
{
	CApplication *m_pApp = new CApp();
	
	if( FAILED(m_pApp->Initialize(hInst, lpCmdLine)) )
		return 0;

	return m_pApp->Run();
}
