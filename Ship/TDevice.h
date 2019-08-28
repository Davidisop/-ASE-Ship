#pragma once
#include "xStd.h"
class TDevice
{
public:
	ID3D11Device *			m_pd3dDevice;	// 디바이스 객체
	ID3D11DeviceContext*	m_pContext;		// 디바이스 컨텍스트
	IDXGISwapChain*			m_pSwapChain;	//WINAPI의 더블버퍼링이랑 같음.백버퍼를 만듬.// 스왑체인 객체
	IDXGIFactory*			m_pDXGIFactory;	// DXGI 객체

	ID3D11RenderTargetView*	m_pRenderTargetView;	// 메인 랜더타켓 뷰
	DXGI_SWAP_CHAIN_DESC	m_sd;
public:
	HRESULT CreateDevice();
	HRESULT CreateDX11GIFactory();
	HRESULT CreateSwapChain();
	HRESULT SetRenderTarget();
	void SetViewPort();

public:
	virtual void DeleteDeviceResources(UINT iWidth, UINT iHeight);
	virtual HRESULT CreateDeviceResources(UINT iWidth, UINT iHeight);

	HRESULT ResizeDevice(UINT iWidth, UINT iHeight);
public:
	virtual bool Init();
	virtual bool Frame() { return true; }
	virtual bool PreRender();
	virtual bool PostRender();
	virtual bool Render();
	virtual bool Release();
public:
	TDevice();
	virtual ~TDevice();
};

