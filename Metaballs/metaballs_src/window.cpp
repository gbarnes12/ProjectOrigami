//-----------------------------------------------------------------------------
// Copyright (c) 2001 Andreas Jönsson
//-----------------------------------------------------------------------------
#include "window.h"
#include "application.h"

//-----------------------------------------------------------------------------
// CWindow()
//-----------------------------------------------------------------------------
CWindow::CWindow()
{
	m_pApplication = 0;
	m_hWnd         = 0;
}

//-----------------------------------------------------------------------------
// SetDefault()
//-----------------------------------------------------------------------------
void CWindow::SetDefault(SParameters *pParams)
{
	pParams->m_hInstance   = 0;
	pParams->m_bFullscreen = false;
	pParams->m_nHeight     = 480;
	pParams->m_nWidth      = 640;
	pParams->m_sTitle      = "Window";
}

//-----------------------------------------------------------------------------
// Create()
//-----------------------------------------------------------------------------
HRESULT CWindow::Initialize(SParameters &Params)
{
	// Get a reference to the application for future use
	m_pApplication = CApplication::GetInstance();
	m_bFullscreen  = Params.m_bFullscreen;

    // Register the window class
    WNDCLASS WndClass;
	WndClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	WndClass.lpfnWndProc   = (WNDPROC)WndProc;
	WndClass.cbClsExtra    = 0;
	WndClass.cbWndExtra    = 0;
	WndClass.hInstance     = Params.m_hInstance;
	WndClass.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
	WndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); 
	WndClass.lpszMenuName  = NULL;
	WndClass.lpszClassName = "Window";

    if( !RegisterClass( &WndClass ) )
	{
		return E_FAIL;
	}

    // Setup our window parameters
	int PosX, PosY, Width, Height;
	DWORD ExStyle, Style;
	if( Params.m_bFullscreen )
	{
		ExStyle = WS_EX_TOPMOST; // This lets our window cover the whole screen
		Style   = WS_POPUP;      // We don't need caption and system 
		                         // menu when in fullscreen mode

		// Set the window to cover the whole screen
		PosX   = 0;
		PosY   = 0;
		Width  = GetSystemMetrics(SM_CXSCREEN);
		Height = GetSystemMetrics(SM_CYSCREEN);
	}
	else
	{
		ExStyle = 0;
		Style   = WS_OVERLAPPEDWINDOW; // This is a normal window

		// Make sure the client area is the size we want
		RECT Rect = {0, 0, Params.m_nWidth, Params.m_nHeight};
		AdjustWindowRect(&Rect, Style, FALSE);

		PosX   = CW_USEDEFAULT; // Let Windows position our window
		PosY   = CW_USEDEFAULT;
		Width  = Rect.right - Rect.left;
		Height = Rect.bottom - Rect.top;
	}


    // Create the render window
	// Set lpParam to point to our application so that the window procedure
	// knows which class to call when handling window messages
    m_hWnd = CreateWindowEx( ExStyle, "Window", 
		                     Params.m_sTitle, Style, PosX, PosY,
						     Width,Height,NULL,NULL,Params.m_hInstance,this ); 
	if( m_hWnd == NULL )
	{
		return E_FAIL;
	}

	// Show the window
	ShowWindow( m_hWnd, SW_SHOW );
    UpdateWindow( m_hWnd );

	return S_OK;
}


//-----------------------------------------------------------------------------
// MsgProc()
//-----------------------------------------------------------------------------
LRESULT CWindow::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
    {
	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE || HIWORD(wParam) )
			m_pApplication->Activate(false);
		else
			m_pApplication->Activate(true);
		return 0;

	case WM_PAINT:
		m_pApplication->Render();
		ValidateRect(hWnd, NULL);
		return 0;

    case WM_SIZE:
		m_pApplication->Resize(LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_ENTERSIZEMOVE:
		// We should pause here as Windows won't allow us to 
		// do anything while the user moves or resizes the window
		m_pApplication->Pause(true);
		return 0;

	case WM_EXITSIZEMOVE:
		m_pApplication->Pause(false);
		return 0;

	case WM_SETCURSOR:
        // Prevent a cursor in fullscreen mode
        if( m_bFullscreen )
        {
            SetCursor(NULL);
            return TRUE;
        }
        break;

    case WM_SYSCOMMAND:
        // Prevent moving/sizing and power loss in fullscreen mode
        switch( wParam )
        {
            case SC_MOVE:
            case SC_SIZE:
            case SC_MAXIMIZE:
			case SC_SCREENSAVE:
            case SC_MONITORPOWER:
                if( m_bFullscreen )
                    return 0;
                break;
        }
        break;

	case WM_KEYDOWN:
		// Some standard keys
		switch( wParam )
		{
		case VK_ESCAPE:
			DestroyWindow(m_hWnd);
			break;
		}
		break;

	case WM_DESTROY:
		// Tell the message loop to quit
		PostQuitMessage(0);
		return 0;
	}

	// Let the default window procedure handle 
	// any message that we don't care about
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}



//-----------------------------------------------------------------------------
// WndProc()
// Static msg handler which passes messages to the application class.
//-----------------------------------------------------------------------------
LRESULT CALLBACK CWindow::WndProc( HWND hWnd, UINT uMsg, 
								   WPARAM wParam, LPARAM lParam )
{
	CWindow *pWindow;

	if( uMsg == WM_CREATE )
	{
		// Extract the window pointer parameter.
		CREATESTRUCT *pCS = (CREATESTRUCT *)lParam;
		pWindow = (CWindow *)pCS->lpCreateParams;

		if( pWindow )
		{
			// Save it in the window handle.
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)pWindow);
		}
	}
	else
	{
		// Extract the object from the window handle.
		pWindow = (CWindow *)GetWindowLong(hWnd, GWL_USERDATA);
	}

	if( pWindow )
	{
		// Let the object handle it.
		return pWindow->MsgProc(hWnd, uMsg, wParam, lParam);
	}
	else
	{
		// Just do the default if the window is not created yet.
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}


//-----------------------------------------------------------------------------
// CheckMessage()
// If bWait is true then the function will wait until a message arrives.
// The function returns true if it receives WM_QUIT.
//-----------------------------------------------------------------------------
bool CWindow::CheckMessage(bool bWait)
{
    MSG  Msg;
    
	if( bWait )
		WaitMessage();

	while( PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE) )
    {
		if( Msg.message == WM_QUIT )
			return true;

        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return false;
}

//-----------------------------------------------------------------------------
// GetHandle()
//-----------------------------------------------------------------------------
HWND CWindow::GetHandle()
{
	return m_hWnd;
}
