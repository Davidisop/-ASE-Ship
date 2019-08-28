#pragma once
#include "xWindow.h"
#include "TTimer.h"


struct TOutput									//모니터의 정보와 갯수를 알수있다.
{
	DXGI_OUTPUT_DESC				desc;
	std::vector<DXGI_MODE_DESC>		m_pModeList;
	std::vector<IDXGIOutput*>		m_pOutputList;
};
struct TAdapter									//그래픽카드
{
	IDXGIAdapter*				m_pAdapter;
	DXGI_ADAPTER_DESC			desc;
	std::vector<TOutput>		m_OutputList;
};

class xCore : public xWindow
{
public:
	TTimer			m_Timer;

	
public:
	bool GamePreInit();

	bool GameInit();
	bool GameRun();
	bool GameRelease();

	bool GameFrame();
	bool GameRender();
	bool GamePreRender();
	bool GamePostRender();
	void DeleteDeviceResources(UINT iWidth, UINT iHeight);
	HRESULT CreateDeviceResources(UINT iWidth, UINT iHeight);

public:
	virtual bool Init();
	virtual bool Frame();
	virtual bool PreRender();
	virtual bool PostRender();
	virtual bool Render();
	virtual bool Release();

public:
	xCore();
	virtual ~xCore();
};

