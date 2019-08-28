#include "xWindow.h"

xWindow * g_pWindow = NULL;
HWND g_hWnd = NULL;
HINSTANCE g_hInstance = NULL;
RECT g_rtClient;
HDC  g_hOffScreenDC;
bool g_bActiveApp = false;


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//DEBUG시에만 작동한다. 조건문이 false이면 예외발생.
	assert(g_pWindow != NULL);

	LRESULT ret;

	if (ret = g_pWindow->MsgProc(hWnd, msg, wParam, lParam))
	{
		return ret;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT xWindow::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ACTIVATEAPP:
		g_bActiveApp = (BOOL)wParam; //wParam에 TRUE가 들어오면 활성화.
		break;

	case WM_SIZE:
		{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);
			ResizeDevice(width, height);
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
			// 전체화면과 윈도우 모드 토글 변환(백 버퍼의 크기는 변화가 없다.)
		case '0':
			{
				BOOL IsScreenMode = false;
				m_pSwapChain->GetFullscreenState(&IsScreenMode, NULL);
				m_pSwapChain->SetFullscreenState(!IsScreenMode, NULL);
				if (IsScreenMode)
				{
					ShowWindow(m_hWnd, SW_SHOW);
				}
			}
			break;
		case '1':// 윈도우 크기(해상도) 및 포맷 변경
			if (m_pSwapChain)
			{
				DXGI_MODE_DESC desc;
				ZeroMemory(&desc, sizeof(desc));
				desc.Width = 1024;
				desc.Height = 768;
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.RefreshRate.Denominator = 0;
				desc.RefreshRate.Numerator = 0;
				desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
				desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				// 윈도우 크기 및 해상도( 화면모드 ) 변경
				m_pSwapChain->ResizeTarget(&desc);
			}
			break;
		case '2':// 전체화면 모드로 전환 및 해상도 변경
			if (m_pSwapChain)
			{
				DXGI_MODE_DESC desc;
				ZeroMemory(&desc, sizeof(desc));
				desc.Width = 800;
				desc.Height = 600;
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.RefreshRate.Denominator = 0;
				desc.RefreshRate.Numerator = 0;
				desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
				desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

				// 윈도우 크기(해상도) 변경
				m_pSwapChain->ResizeTarget(&desc);
				m_pSwapChain->SetFullscreenState(TRUE, NULL);
			}
			break;
		}
		break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);		// WM_QUIT 메세지가 메시지큐에 푸쉬된다.
								//DestroyWindow(hWnd);	// WM_DESTROY 메세지가 메세지큐에 푸쉬된다.(다른버튼으로 종료할때)
	}
	break;
	return 0;
	//default:
	//	return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

void xWindow::MsgEvent(MSG msg)
{
	I_Input.MsgEvent(msg);
}

bool xWindow::SetWindow(HINSTANCE hInstance, const TCHAR *pTitleName,
	UINT iWidth, UINT iHeight)
{

	m_hInstance = hInstance;
	g_hInstance = hInstance;

	//윈도우 클래스 등록
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = m_hInstance;
	wc.lpszClassName = L"MyWindow";
	wc.hIcon = LoadIcon(m_hInstance, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	//wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.hIconSm = LoadIcon(NULL, IDI_HAND);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;



	if (RegisterClassEx(&wc) == FALSE)
	{
		assert(RegisterClassEx(&wc) != FALSE);
		return false;
	}

	//클라이언트 영역 수정(클라이언트 영역을 정확하게, (800, 600)으로 수정한다)
	RECT rt{ 0,0, iWidth, iHeight };
	AdjustWindowRect(&rt, m_dwStyle, FALSE);

	//윈도우 생성(등록된 클래스를 사용해서)
	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW,
		L"MyWindow", pTitleName, m_dwStyle,
		0, 0, rt.right - rt.left, rt.bottom - rt.top, NULL, NULL, m_hInstance, NULL);

	if (m_hWnd == NULL)
	{
		assert(m_hWnd != NULL);
		return false;
	}

	g_hWnd = m_hWnd;

	

	m_hScreenDC = GetDC(m_hWnd);
	m_hOffScreenDC = CreateCompatibleDC(m_hScreenDC);
	g_hOffScreenDC = m_hOffScreenDC;

	GetWindowRect(m_hWnd, &m_rtWindow);
	GetClientRect(m_hWnd, &m_rtClient);
	g_rtClient = m_rtClient;

	CenterWindow();

	ShowWindow(m_hWnd, SW_SHOW);
	return true;
}
void xWindow::Set(DWORD dwStyle)
{
	m_dwStyle = dwStyle;
}

void xWindow::CenterWindow()
{
	assert(m_hWnd != NULL);

	int iScreenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
	int iScreenHeigth = GetSystemMetrics(SM_CYFULLSCREEN);

	int x = (iScreenWidth - (m_rtWindow.right - m_rtWindow.left)) / 2;
	int y = (iScreenHeigth - (m_rtWindow.bottom - m_rtWindow.top)) / 2;

	TCHAR	csBuffer[256];
	_stprintf_s(csBuffer, L"m_hWnd = %p, iScreenWidth  = %d, iScreenHeigth = %d, x = %d, y = %d, windowWidth = %d, windowHeight = %d\n",
		m_hWnd, iScreenWidth, iScreenHeigth, x, y, (m_rtWindow.right - m_rtWindow.left), (m_rtWindow.bottom - m_rtWindow.top));

	OutputDebugString(csBuffer);
	MoveWindow(m_hWnd, x, y, m_rtWindow.right, m_rtWindow.bottom, true);
}
bool xWindow::Run()
{
	if (!GameInit())
		return false;

	//GetMessage(&msg, NULL, 0, 0)
	//메세지큐에서 메세지가 1개이상 있을 경우 1개를 가져옴.
	//PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)
	//메세지큐에서 메세지 있으면 메세지 반환, 없으면 false를 반환.
	MSG msg;		// WM_QUIT
	ZeroMemory(&msg, sizeof(MSG));

	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				break;
			}
			MsgEvent(msg);
		}
		else
		{
			GameRun();
			Sleep(1);
		}
	}

	return GameRelease();
}

bool xWindow::GameInit()
{
	TDevice::Init();
	return true;
}

bool xWindow::GameRun()
{
	return true;
}

bool xWindow::GameRelease()
{
	return true;
}
xWindow::xWindow() : m_dwStyle(WS_OVERLAPPEDWINDOW)
{
	g_pWindow = this;
	//m_dwStyle = WS_OVERLAPPEDWINDOW;
}


xWindow::~xWindow()
{
}

