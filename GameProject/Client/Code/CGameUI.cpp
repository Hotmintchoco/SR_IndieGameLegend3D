#include "pch.h"
#include "CGameUI.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"

CGameUI::CGameUI(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}


CGameUI::~CGameUI()
{
}

HRESULT CGameUI::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    __super::Ready_GameObject();

    return S_OK;
}

_int CGameUI::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return iExit;
}

void CGameUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    _vec3       vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    CGameObject::Compute_ViewZ(&vPos);
}

void CGameUI::Render_GameObject()
{

}

HRESULT CGameUI::Add_Component()
{
    CComponent* pComponent = nullptr;

    // RcCol
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    return S_OK;
}

CGameUI* CGameUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CGameUI* pGameUI = new CGameUI(pGraphicDev);

    if (FAILED(pGameUI->Ready_GameObject()))
    {
        Safe_Release(pGameUI);
        MSG_BOX("CGameUI Create Failed");
        return nullptr;
    }

    return pGameUI;
}

void CGameUI::Free()
{
    CGameObject::Free();
}
