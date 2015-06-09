//-----------------------------------------------------------------------------
// CWindow
//
// This class is an interface for the operative system's windows.
//
// To compile with this class you must link with:
// user32.lib gdi32.lib
//
// Copyright (c) 2001 Andreas Jönsson
//-----------------------------------------------------------------------------
#ifndef WINDOW_H
#define WINDOW_H

#define STRICT				// Enable strict type checking
#define WIN32_LEAN_AND_MEAN // Don't include stuff that are rarely used
#include <windows.h>

class CApplication;

class CWindow
{
public:
	struct SParameters
	{
		HINSTANCE m_hInstance;
		bool      m_bFullscreen;
		int       m_nWidth;
		int       m_nHeight;
		char     *m_sTitle;
	};

	CWindow();

	HRESULT Initialize(SParameters &Params);
	bool    CheckMessage(bool bWait);
	void    SetDefault(SParameters *pParams);
	HWND    GetHandle();

protected:
    virtual LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HWND          m_hWnd;
	CApplication *m_pApplication;
	bool          m_bFullscreen;

private:
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
};

#endif