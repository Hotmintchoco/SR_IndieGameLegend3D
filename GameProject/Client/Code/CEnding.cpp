#include "pch.h"
#include "CEnding.h"
#include "CBackGround.h"
#include "CFontMgr.h"
#include "CLayer.h"
#include "CProtoMgr.h"
#include "CDInputMgr.h"
#include "CManagement.h"
#include "CSkyBox.h"
#include "CTimerMgr.h"
#include "CSoundMgr.h"
#include "CIntro_MG1.h"
#include "CEnding_Object.h"

CEnding::CEnding(LPDIRECT3DDEVICE9 pGraphicDev)
    : CScene(pGraphicDev)
{
}

CEnding::~CEnding()
{
}

HRESULT CEnding::Ready_Scene()
{
    if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
        return E_FAIL;

    _vec3 vEye(0.f, 4.f, 0.f);
    _vec3 vAt(0.f, 0.f, 0.f);
    _vec3 vUp(0.f, 0.f, 1.f);

    _float fFov = D3DXToRadian(60.f);
    _float fAspect = 16.f / 9.f;
    _float fNear = 0.1f;
    _float fFar = 1000.f;

    _matrix matView, matProj;

    D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
    D3DXMatrixPerspectiveFovLH(&matProj, fFov, fAspect, fNear, fFar);

    m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
    m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

    return S_OK;
}

_int CEnding::Update_Scene(_float fTimeDelta)
{
    _int iExit = CScene::Update_Scene(fTimeDelta);
    m_fElapsedTime += fTimeDelta;


   
    return iExit;
}

void CEnding::LateUpdate_Scene(_float fTimeDelta)
{
    CScene::LateUpdate_Scene(fTimeDelta);
}

void CEnding::Render_Scene()
{

}

HRESULT CEnding::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
    CLayer* pLayer = CLayer::Create();
    if (nullptr == pLayer) return E_FAIL;

    m_mapLayer.insert({ pLayerTag ,pLayer });

	CGameObject* pGameObject = nullptr;

    // SkyBox
    pGameObject = CSkyBox::Create(m_pGraphicDev);
    if (nullptr == pGameObject) return E_FAIL;
    if (FAILED(pLayer->Add_GameObject(L"SkyBox", pGameObject))) return E_FAIL;


    for (_int i = 0; i < (_int)m_vC.size(); ++i)
	{
        _vec3 vPos = m_vCStart[i];
        _vec3 vDst = m_vC[i];
		pGameObject = CEnding_Object::Create(m_pGraphicDev, 0, vPos, vDst);
		if (nullptr == pGameObject) return E_FAIL;
		if (FAILED(pLayer->Add_GameObject(L"C", pGameObject))) return E_FAIL;
    }
    for (_int i = 0; i < (_int)m_vL.size(); ++i)
    {
        _vec3 vPos = m_vLStart[i];
        _vec3 vDst = m_vL[i];
        pGameObject = CEnding_Object::Create(m_pGraphicDev, 1, vPos, vDst);
        if (nullptr == pGameObject) return E_FAIL;
        if (FAILED(pLayer->Add_GameObject(L"L", pGameObject))) return E_FAIL;
    }
    for (_int i = 0; i < (_int)m_vE.size(); ++i)
    {
        _vec3 vPos = m_vEStart[i];
        _vec3 vDst = m_vE[i];
        pGameObject = CEnding_Object::Create(m_pGraphicDev, 2, vPos, vDst);
        if (nullptr == pGameObject) return E_FAIL;
        if (FAILED(pLayer->Add_GameObject(L"E", pGameObject))) return E_FAIL;
    }
    for (_int i = 0; i < (_int)m_vA.size(); ++i)
    {
        _vec3 vPos = m_vAStart[i];
        _vec3 vDst = m_vA[i];
        pGameObject = CEnding_Object::Create(m_pGraphicDev, 3, vPos, vDst);
        if (nullptr == pGameObject) return E_FAIL;
        if (FAILED(pLayer->Add_GameObject(L"A", pGameObject))) return E_FAIL;
    }
    for (_int i = 0; i < (_int)m_vR.size(); ++i)
    {
        _vec3 vPos = m_vRStart[i];
        _vec3 vDst = m_vR[i];
        pGameObject = CEnding_Object::Create(m_pGraphicDev, 4, vPos, vDst);
        if (nullptr == pGameObject) return E_FAIL;
        if (FAILED(pLayer->Add_GameObject(L"R", pGameObject))) return E_FAIL;
    }



    pGameObject = CIntro_MG1::Create(m_pGraphicDev);
    if (nullptr == pGameObject) return E_FAIL;
    if (FAILED(pLayer->Add_GameObject(L"Intro", pGameObject))) return E_FAIL;

    return S_OK;
}

CEnding* CEnding::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CEnding* pEnding = new CEnding(pGraphicDev);
    if (FAILED(pEnding->Ready_Scene()))
    {
        Safe_Release(pEnding);
        MSG_BOX("CEnding Create Failed");
        return nullptr;
    }

    return pEnding;
}

void CEnding::Free()
{
    CScene::Free();
}