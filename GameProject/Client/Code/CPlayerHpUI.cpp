#include "pch.h"
#include "CPlayerHpUI.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"

CPlayerHpUI::CPlayerHpUI(LPDIRECT3DDEVICE9 pGraphicDev)
    : CUI(pGraphicDev)
{
}


CPlayerHpUI::~CPlayerHpUI()
{
}

HRESULT CPlayerHpUI::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    CGameObject::Ready_GameObject();

	m_pTransformCom->Set_Scale(15.f, 15.f, 1.f);

    return S_OK;
}

_int CPlayerHpUI::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return iExit;
}

void CPlayerHpUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    _vec3       vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    CGameObject::Compute_ViewZ(&vPos);
}

void CPlayerHpUI::Render_GameObject()
{
    CUI::Render_GameObject();

}

HRESULT CPlayerHpUI::Add_Component()
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
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_HpUITexture"));

    if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CPlayerHpUI* CPlayerHpUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CPlayerHpUI* pGameUI = new CPlayerHpUI(pGraphicDev);

    if (FAILED(pGameUI->Ready_GameObject()))
    {
        Safe_Release(pGameUI);
        MSG_BOX("CPlayerHpUI Create Failed");
        return nullptr;
    }

    return pGameUI;
}

void CPlayerHpUI::Free()
{
    CGameObject::Free();
}
