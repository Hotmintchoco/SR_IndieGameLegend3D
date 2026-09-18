#include "pch.h"
#include "CMainApp.h"
#include "CTimerMgr.h"
#include "CFrameMgr.h"
#include "CFontMgr.h"
#include "CLogo.h"
#include "CProtoMgr.h"
#include "CStage.h"
#include "CDInputMgr.h"
#include "CLightMgr.h"
#include "CCollisionMgr.h"
#include "CCameraMgr.h"
#include "CImGuiTool.h"
#include "CRoomLoadingMgr.h"
#include "CAbstractFactory.h"
#include "CGameStatusMgr.h"
#include "CDebugMgr.h"
#include "CRandomMgr.h"

CMainApp::CMainApp() : m_pDeviceClass(nullptr), m_pGraphicDev(nullptr)
, m_pManagementClass(CManagement::GetInstance())
{
}

CMainApp::~CMainApp()
{
}

HRESULT CMainApp::Ready_MainApp()
{
	srand((_uint)time(nullptr));
	if (FAILED(Ready_DefaultSetting(&m_pGraphicDev)))
		return E_FAIL;

	if (FAILED(Ready_Scene(m_pGraphicDev)))
		return E_FAIL;

	if (FAILED(CImGuiTool::Ready(g_hWnd, m_pGraphicDev)))
		return E_FAIL;

	return S_OK;
}

int CMainApp::Update_MainApp(const _float& fTimeDelta)
{
	CDInputMgr::GetInstance()->Update_InputDev();

	m_pManagementClass->Update_Scene(fTimeDelta);

	CGameStatusMgr::GetInstance()->Update(fTimeDelta);
	CDebugMgr::GetInstance()->Update(fTimeDelta);

	return 0;
}

void CMainApp::LateUpdate_MainApp(const _float& fTimeDelta)
{
	m_pManagementClass->LateUpdate_Scene(fTimeDelta);
}

void CMainApp::Render_MainApp()
{
	m_pDeviceClass->Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));

	CImGuiTool::BeginFrame();

	m_pManagementClass->Render_Scene(m_pGraphicDev);

	CGameStatusMgr::GetInstance()->Render();
	CDebugMgr::GetInstance()->Render();

	CImGuiTool::EndFrame();

	m_pDeviceClass->Render_End();

}

HRESULT CMainApp::Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev)
{
	if (FAILED(CGraphicDev::GetInstance()->Ready_GraphicDev(g_hWnd, MODE_WIN, WINCX, WINCY,
		&m_pDeviceClass)))
	{
		MSG_BOX("GraphicDev Get Failed");
		return E_FAIL;
	}

	m_pDeviceClass->AddRef();

	(*ppGraphicDev) = m_pDeviceClass->Get_GraphicDev();

	// 폰트 추가

	if (FAILED(CFontMgr::GetInstance()->Ready_Font((*ppGraphicDev), L"Font_Default", L"바탕", 20, 20, FW_HEAVY)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font((*ppGraphicDev), L"Font_Jinji", L"궁서", 15, 15, FW_THIN)))
		return E_FAIL;

	(*ppGraphicDev)->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 마우스 초기화

	if (FAILED(CDInputMgr::GetInstance()->Ready_InputDev(g_hInst, g_hWnd)))
		return E_FAIL;

	// 필터링 적용
	/* 도트 기반 텍스쳐라 끕니다 */
	//(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CScene* pLogo = CLogo::Create(pGraphicDev);

	if (nullptr == pLogo)
		return E_FAIL;

	if (FAILED(m_pManagementClass->Set_Scene(pLogo)))
	{
		Safe_Release(pLogo);
		MSG_BOX("Logo Create Failed");
		return E_FAIL;
	}

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pMainApp = new CMainApp;

	if (FAILED(pMainApp->Ready_MainApp()))
	{
		delete pMainApp;
		pMainApp = nullptr;

		return nullptr;
	}

	return pMainApp;
}

void CMainApp::Free()
{
	Safe_Release(m_pDeviceClass);

	CCollisionMgr::DestroyInstance();
	CLightMgr::DestroyInstance();
	CRenderer::DestroyInstance();
	CDInputMgr::DestroyInstance();
	CProtoMgr::DestroyInstance();
	CFontMgr::DestroyInstance();
	CFrameMgr::DestroyInstance();
	CTimerMgr::DestroyInstance();
	CCameraMgr::DestroyInstance();
	CRoomLoadingMgr::DestroyInstance();
	CAbstractFactory::DestroyInstance();
	CGameStatusMgr::DestroyInstance();
	CDebugMgr::DestroyInstance();
	CRandomMgr::DestroyInstance();

	m_pManagementClass->DestroyInstance();
	m_pDeviceClass->DestroyInstance();

	CImGuiTool::Release();
}
