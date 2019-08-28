#include "xCore.h"


bool xCore::Init()
{
	xWindow::Init();
	m_Timer.Init();
	
	I_Input.Init();

	DeleteDeviceResources(m_sd.BufferDesc.Width, m_sd.BufferDesc.Height);
	CreateDeviceResources(m_sd.BufferDesc.Width, m_sd.BufferDesc.Height);
	return true;
}
bool xCore::Frame()
{
	xWindow::Frame();
	m_Timer.Frame();

	I_Input.Frame();
	return true;
}

bool xCore::PreRender()
{
	return xWindow::PreRender();
}
bool xCore::PostRender()
{
	return xWindow::PostRender();
}
bool xCore::Render()
{
	xWindow::Render();
	m_Timer.Render();

	I_Input.Render();
	return true;
}
bool xCore::Release()
{
	xWindow::Release();
	m_Timer.Release();
	
	I_Input.Release();
	return true;
}

bool xCore::GamePreInit()
{
	HRESULT hr;

	TDevice::CreateDX11GIFactory();
	std::vector<TAdapter> m_pTAdapterList;		//그래픽 카드 정보와 갯수를 알수있다.

	for (int iAdapter = 0; ; iAdapter++)			//그래픽카드 찾는다.
	{
		IDXGIAdapter* pAdapter = NULL;
		TAdapter tAdapter;

		hr = m_pDXGIFactory->EnumAdapters(iAdapter, &pAdapter);

		if (FAILED(hr)) break;

		pAdapter->GetDesc(&tAdapter.desc);
		tAdapter.m_pAdapter = pAdapter;
		m_pTAdapterList.push_back(tAdapter);
	}

	for (int iAdapter = 0; iAdapter < m_pTAdapterList.size(); iAdapter++)
	{
		DXGI_MODE_DESC desc[512];

		for (int iOutput = 0; ; iOutput++)	//그래픽카드 별로 연결된 모니터를 찾는다.
		{
			TOutput Output;
			IDXGIOutput* pOutput = NULL;

			hr = m_pTAdapterList.at(iAdapter).m_pAdapter->EnumOutputs(iOutput, &pOutput);
			if (pOutput == NULL || FAILED(hr)) break;

			if (pOutput != NULL)
			{
				pOutput->GetDesc(&Output.desc);
				UINT NumModes = 512;

				hr = pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_SCALING, &NumModes, desc);
			}

			Output.m_pOutputList.push_back(pOutput);

			m_pTAdapterList.at(iAdapter).m_OutputList.push_back(Output);

			for (int iMode = 0; iMode < 512; iMode++)
			{
				if (desc[iMode].Width >= 800 && desc[iMode].Width <= 1920 &&
					desc[iMode].Height >= 600 && desc[iMode].Height <= 1080)
				{
					m_pTAdapterList.at(iAdapter).m_OutputList[iOutput].m_pModeList.push_back(desc[iMode]);
				}
			}
		}
	}
	return true;
}

bool xCore::GameInit()
{
	GamePreInit();
	Init();

	return true;
}

bool xCore::GameFrame()
{
	Frame();

	return true;
}

bool xCore::GameRender()
{
	GamePreRender();
	{
		PreRender();
		Render();
		PostRender();
	}
	GamePostRender();
	return true;
}

bool xCore::GamePreRender()
{
	return true;
}
bool xCore::GamePostRender()
{
	return true;
}

void xCore::DeleteDeviceResources(UINT iWidth, UINT iHeight)
{
	
}
HRESULT xCore::CreateDeviceResources(UINT iWidth, UINT iHeight)
{
	IDXGISurface1*		pBackBuffer;

	HRESULT hr = m_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), (void**)&pBackBuffer);
	
	
	if (pBackBuffer)	pBackBuffer->Release();
	
	return hr;
}
bool xCore::GameRun()
{
	GameFrame();
	GameRender();

	return true;
}
bool xCore::GameRelease()
{
	Release();
	return true;
}
xCore::xCore()
{
}


xCore::~xCore()
{
}
