#ifndef APP_H
#define APP_H

#include "application.h"
#include "metaballs.h"
#include "timer.h"
#include <d3d8.h>

class CApp : public CApplication
{
public:
	CApp();

	HRESULT Initialize(HINSTANCE hInst, char *sCmdLine);
	int     Run();

protected:
	CMetaballs *m_pMetaballs;
	CTimer     *m_pTimer;

	IDirect3DTexture8 *m_pTexture;
};

#endif