#pragma once
#include "TDevice.h"
#include "TInput.h"

class xWindow :public TDevice
{
public:
	HWND			m_hWnd;
	HINSTANCE		m_hInstance;
	RECT			m_rtClient;
	RECT			m_rtWindow;
public:
	HDC				m_hScreenDC;
	HDC				m_hOffScreenDC;

private:
	DWORD			m_dwStyle;

public:
	void Set(DWORD dwStyle);
	void CenterWindow();

	bool SetWindow(HINSTANCE hInstance, const TCHAR *pTitleName = L"KGCA Sample",
		UINT iWidth = 900, UINT iHeight = 500);

	bool Run();

public:
	virtual LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual bool GameInit();
	virtual bool GameRun();
	virtual bool GameRelease();
	void MsgEvent(MSG msg);
public:
	xWindow();
	virtual ~xWindow();
};

