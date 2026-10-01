#include "pch.h"
#include "CGaugeUI.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CStage.h"
#include "CGameStatus.h"

CGaugeUI::CGaugeUI(LPDIRECT3DDEVICE9 pGraphicDev)
    : CUI(pGraphicDev)
{
}


CGaugeUI::~CGaugeUI()
{
}

HRESULT CGaugeUI::Ready_GameObject()
{
    if (FAILED(__super::Add_Component()))
        return E_FAIL;

    CGameObject::Ready_GameObject();

    m_pTransformCom->Set_Scale(15.f, 15.f, 1.f);

    return S_OK;
}

_int CGaugeUI::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return iExit;
}

void CGaugeUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    _vec3       vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    CGameObject::Compute_ViewZ(&vPos);
}

void CGaugeUI::Render_GameObject()
{
    if (m_bHorizontal)
        Render_HorizontalGauge();
    else
		Render_VerticalGauge();
}

void CGaugeUI::Render_HorizontalGauge()
{
    RECT rcClip;
    rcClip.left = LONG(m_vPos.x - m_vSize.x);
    rcClip.top = LONG(m_vPos.y - m_vSize.y);
    rcClip.right = LONG((m_vPos.x - m_vSize.x) + m_vSize.x * 2.f * m_fPercent);
    rcClip.bottom = LONG(m_vPos.y + m_vSize.y);
    m_pGraphicDev->SetScissorRect(&rcClip);
    m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);

    CUI::Render_GameObject();

    m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
}

void CGaugeUI::Render_VerticalGauge()
{
    RECT rcClip;
    rcClip.left = LONG(m_vPos.x - m_vSize.x);
    rcClip.top = LONG((m_vPos.y + m_vSize.y) - m_vSize.y * 2.f * m_fPercent);
    rcClip.right = LONG(m_vPos.x + m_vSize.x);
    rcClip.bottom = LONG(m_vPos.y + m_vSize.y);
    m_pGraphicDev->SetScissorRect(&rcClip);
    m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);

    CUI::Render_GameObject();

    m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
}

CGaugeUI* CGaugeUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CGaugeUI* pGameUI = new CGaugeUI(pGraphicDev);

    if (FAILED(pGameUI->Ready_GameObject()))
    {
        Safe_Release(pGameUI);
        MSG_BOX("CGaugeUI Create Failed");
        return nullptr;
    }

    return pGameUI;
}

CGaugeUI* CGaugeUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrTextureTag, _bool bHorizontal)
{
    CGaugeUI* pGameUI = new CGaugeUI(pGraphicDev);
    pGameUI->m_wstrTextureTag = wstrTextureTag;
	pGameUI->m_bHorizontal = bHorizontal;

    if (FAILED(pGameUI->Ready_GameObject()))
    {
        Safe_Release(pGameUI);
        MSG_BOX("CGaugeUI Create Failed");
        return nullptr;
    }

    return pGameUI;
}

void CGaugeUI::Free()
{
    CGameObject::Free();
}
