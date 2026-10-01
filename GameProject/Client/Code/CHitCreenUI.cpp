#include "pch.h"
#include "CHitCreenUI.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CCameraMgr.h"

CHitCreenUI::CHitCreenUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev), m_fAlpha(0.f)
{
}


CHitCreenUI::~CHitCreenUI()
{
}

HRESULT CHitCreenUI::Ready_GameObject()
{
    if (FAILED(__super::Add_Component()))
        return E_FAIL;

    CGameObject::Ready_GameObject();

    return S_OK;
}

_int CHitCreenUI::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    // 알파값이 0보다 크면 서서히 감소시킴 (페이드 아웃)
    if (m_fAlpha > 0.f)
    {
        // 초당 약 255씩 감소 (1초 만에 완전히 사라짐, 속도는 조절 가능)
        m_fAlpha -= 255.f * fTimeDelta;
    
        if (m_fAlpha < 0.f)
            m_fAlpha = 0.f;
    }

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return iExit;
}

void CHitCreenUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    _vec3       vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    CGameObject::Compute_ViewZ(&vPos);
}

void CHitCreenUI::Render_GameObject()
{
    // 알파값이 0 이하면 그릴 필요가 없음
    if (m_fAlpha <= 0.f) return;

    // TFactor 세팅 (텍스처의 원본 색상에 m_fAlpha 값을 투명도로 적용)
    D3DCOLOR tintColor = D3DCOLOR_ARGB((int)m_fAlpha, 255, 255, 255);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, tintColor);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(0); // 피격 텍스처 세팅

    // TFactor 연산 설정
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

    m_pBufferCom->Render_Buffer();

    // 렌더 스테이트 원상 복구 (다른 객체 렌더링에 영향을 주지 않도록)
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
}

CHitCreenUI* CHitCreenUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CHitCreenUI* pGameUI = new CHitCreenUI(pGraphicDev);

    if (FAILED(pGameUI->Ready_GameObject()))
    {
        Safe_Release(pGameUI);
        MSG_BOX("CHitCreenUI Create Failed");
        return nullptr;
    }

    return pGameUI;
}

CHitCreenUI* CHitCreenUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrTextureTag)
{
    CHitCreenUI* pGameUI = new CHitCreenUI(pGraphicDev);
    pGameUI->m_wstrTextureTag = wstrTextureTag;

    if (FAILED(pGameUI->Ready_GameObject()))
    {
        Safe_Release(pGameUI);
        MSG_BOX("CCHitCreenUI Create Failed");
        return nullptr;
    }

    return pGameUI;
}


void CHitCreenUI::Free()
{
    CGameObject::Free();
}
