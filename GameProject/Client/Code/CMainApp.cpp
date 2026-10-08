#include "pch.h"
#include "CShaderEffectMgr.h"
#include "CMainApp.h"
#include "CTimerMgr.h"
#include "CFrameMgr.h"
#include "CFontMgr.h"
#include "CStartScreen.h"
#include "CProtoMgr.h"
#include "CStage.h"
#include "CDInputMgr.h"
#include "CLightMgr.h"
#include "CCollisionMgr.h"
#include "CClientCameraMgr.h"
#include "CCamera.h"
#include "CImGuiTool.h"
#include "CRoomLoadingMgr.h"
#include "CAbstractFactory.h"
#include "CRandomMgr.h"
#include "CDebugMgr.h"
#include "CSoundMgr.h"
#include "CUIMgr.h"
#include "CCursorPolicyMgr.h"
#include "CRenderer.h"
#include "CUnderwaterEffect.h"

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

	if (FAILED(CSoundMgr::GetInstance()->Ready()))
		return E_FAIL;

	if (FAILED(Ready_Scene(m_pGraphicDev)))
		return E_FAIL;

	if (FAILED(CImGuiTool::Ready(g_hWnd, m_pGraphicDev)))
		return E_FAIL;

	return S_OK;
}

int CMainApp::Update_MainApp(_float fTimeDelta)
{
	CDInputMgr::GetInstance()->Update_InputDev();

	// 정민 : 쉐이더 왜곡 효과 테스트용
	auto* pEffects = CShaderEffectMgr::GetInstance();
	if (CDInputMgr::GetInstance()->Key_Down(DIK_F6))
	{
        pEffects->Set_PostEffect(pEffects->Get_PostEffectType() == POST_EFFECT::NONE
            ? POST_EFFECT::UNDERWATER : POST_EFFECT::NONE);
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_F7))
	{
		pEffects->Set_PostEffect(pEffects->Get_PostEffectType() == POST_EFFECT::NONE
			? POST_EFFECT::LAVA : POST_EFFECT::NONE);
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_F8))
	{
		pEffects->Set_PostEffect(pEffects->Get_PostEffectType() == POST_EFFECT::NONE
			? POST_EFFECT::INVERT : POST_EFFECT::NONE);
	}


	CShaderEffectMgr::GetInstance()->Update_PostEffect(fTimeDelta);
	CCursorPolicyMgr::GetInstance()->Update();

	m_pManagementClass->Update_Scene(min(fTimeDelta, 0.01f));

	CSoundMgr::GetInstance()->Update();

	return 0;
}

void CMainApp::LateUpdate_MainApp(_float fTimeDelta)
{
	m_pManagementClass->LateUpdate_Scene(min(fTimeDelta, 0.01f));
}

void CMainApp::Render_MainApp()
{
	// Capture before Render_Begin so its clear also clears the offscreen target.
	const bool bPostEffect = dynamic_cast<CStage*>(m_pManagementClass->GetCurrentScene()) &&
		CShaderEffectMgr::GetInstance()->Begin_PostEffect(m_pGraphicDev);
	m_pDeviceClass->Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));

	CImGuiTool::BeginFrame();

	m_pManagementClass->Render_Scene(m_pGraphicDev);
	if (bPostEffect)
		CShaderEffectMgr::GetInstance()->End_PostEffect(m_pGraphicDev);

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
    // Common components are needed by the start screen before loading begins.
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(
        L"Proto_RcTex", Engine::CRcTex::Create(m_pGraphicDev))))
        return E_FAIL;
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(
        L"Proto_Transform", Engine::CTransform::Create(m_pGraphicDev))))
        return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font((*ppGraphicDev), L"Font_Default", L"바탕", 20, 20, FW_HEAVY)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font((*ppGraphicDev), L"Font_Jinji", L"궁서", 15, 15, FW_THIN)))
		return E_FAIL;

	(*ppGraphicDev)->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 마우스 초기화

	if (FAILED(CDInputMgr::GetInstance()->Ready_InputDev(g_hInst, g_hWnd)))
		return E_FAIL;

	// 필터링 적용
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}

HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CScene* pStartScreen = CStartScreen::Create(pGraphicDev);

	if (nullptr == pStartScreen)
		return E_FAIL;

	if (FAILED(m_pManagementClass->Change_Scene(0, pStartScreen, true)))
	{
		Safe_Release(pStartScreen);
		MSG_BOX("Start Screen Create Failed");
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
    CShaderEffectMgr::DestroyInstance();
	Safe_Release(m_pDeviceClass);

	CCollisionMgr::DestroyInstance();
	CLightMgr::DestroyInstance();
	CRenderer::DestroyInstance();
	CDInputMgr::DestroyInstance();
	CProtoMgr::DestroyInstance();
	CFontMgr::DestroyInstance();
	CFrameMgr::DestroyInstance();
	CTimerMgr::DestroyInstance();
	CClientCameraMgr::DestroyInstance();
	CRoomLoadingMgr::DestroyInstance();
	CAbstractFactory::DestroyInstance();
	CRandomMgr::DestroyInstance();
	CDebugMgr::DestroyInstance();
	CSoundMgr::DestroyInstance();
	CUIMgr::DestroyInstance();
	CCursorPolicyMgr::DestroyInstance();

	m_pManagementClass->DestroyInstance();
	m_pDeviceClass->DestroyInstance();

	CImGuiTool::Release();
}
