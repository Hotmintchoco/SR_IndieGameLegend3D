#include "CGraphicDev.h"

IMPLEMENT_SINGLETON(CGraphicDev)

CGraphicDev::CGraphicDev()
	: m_pSDK(nullptr), m_pGraphicDev(nullptr)
{
}

CGraphicDev::~CGraphicDev()
{
	Free();
}

HRESULT CGraphicDev::Ready_GraphicDev(HWND hWnd, 
										WINMODE eMode, 
										const _uint& iSizeX, 
										const _uint& iSizeY, 
										CGraphicDev** ppGraphicDev)
{
	// 1. 성능 조사를 하기 위한 컴객체 생성

	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);


	if (nullptr == m_pSDK)
		return E_FAIL;

	// 2. 그래픽 카드 성능 조사

	D3DCAPS9		DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	// D3DADAPTER_DEFAULT : 네 컴퓨터에 설치된 그래픽 카드(기본 그래픽 카드)

	if (FAILED(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps)))
		return E_FAIL;

	_ulong		dwFlag = 0;

	if (DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		dwFlag |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;

	else
		dwFlag |= D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;


	D3DPRESENT_PARAMETERS		d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	d3dpp.BackBufferWidth = iSizeX;
	d3dpp.BackBufferHeight = iSizeY;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;

	// D3DSWAPEFFECT_FLIP : 버퍼를 하나만 사용
	// D3DSWAPEFFECT_COPY : 더블 버퍼링과 유사
	
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	// 스왑 체인

	d3dpp.hDeviceWindow = hWnd;

	d3dpp.Windowed = MODE_WIN;

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// 3. 그래픽 카드에서 제공되는 기능을 사용할 컴객체 생성

	if (FAILED(m_pSDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, dwFlag, &d3dpp, &m_pGraphicDev)))
		return E_FAIL;

	*ppGraphicDev = this;

	return S_OK;
}

// 후면 버퍼 동작 원리
void CGraphicDev::Render_Begin(D3DXCOLOR Color)
{
	m_pGraphicDev->Clear(0,		// 렉트의 개수
						NULL,	// 여러 개의 렉트 중 첫 번째 렉트의 주소
						D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER,
						Color,	// 후면 버퍼 색상
						1.f,	// 깊이 버퍼 초기화
						0);		// 스텐실 버퍼 초기화

	m_pGraphicDev->BeginScene();

}

void CGraphicDev::Render_End(HWND g_hWnd)
{
	m_pGraphicDev->EndScene();
	m_pGraphicDev->Present(NULL, NULL, g_hWnd, NULL);

	// 1, 2 : 렉트의 주소 // SWAPEFFECT가 COPY일 때 동작
	// 4 : 옵션			// SWAPEFFECT가 COPY일 때 동작
}

void CGraphicDev::Free()
{
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pSDK);
}
