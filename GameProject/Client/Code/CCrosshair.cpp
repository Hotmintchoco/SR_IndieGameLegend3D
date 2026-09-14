#include "pch.h"
#include "CCrosshair.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"

CCrosshair::CCrosshair(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev)
{
}


CCrosshair::~CCrosshair()
{
}

HRESULT CCrosshair::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    CGameObject::Ready_GameObject();

    m_pTransformCom->Set_Scale(15.f, 15.f, 1.f);

    return S_OK;
}

_int CCrosshair::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    m_fFrame += 4.f * fTimeDelta;

    if (4.f < m_fFrame)
        m_fFrame = 0.f;

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return iExit;
}

void CCrosshair::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    _vec3       vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    CGameObject::Compute_ViewZ(&vPos);
}

void CCrosshair::Render_GameObject()
{
    CUI::Render_GameObject();
}

HRESULT CCrosshair::Add_Component()
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

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CrosshairTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CCrosshair* CCrosshair::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CCrosshair* pGameUI = new CCrosshair(pGraphicDev);

    if (FAILED(pGameUI->Ready_GameObject()))
    {
        Safe_Release(pGameUI);
        MSG_BOX("CCrosshair Create Failed");
        return nullptr;
    }

    return pGameUI;
}

void CCrosshair::Free()
{
    CGameObject::Free();
}
