#include "pch.h"
#include "CUI.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"

CUI::CUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_fFrame(0.f)
{
}


CUI::~CUI()
{
}

HRESULT CUI::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CUI::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return iExit;
}

void CUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    _vec3       vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    CGameObject::Compute_ViewZ(&vPos);
}

void CUI::Render_GameObject()
{
    if (nullptr == m_pBufferCom || nullptr == m_pTransformCom)
        return;

    _matrix matOldWorld, matOldView, matOldProj;
    m_pGraphicDev->GetTransform(D3DTS_WORLD, &matOldWorld);
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matOldView);
    m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matOldProj);

    DWORD dwOldZEnable = TRUE;
    DWORD dwOldZWrite = TRUE;
    DWORD dwOldAlphaBlend = FALSE;
    DWORD dwOldCull = D3DCULL_CCW;

    m_pGraphicDev->GetRenderState(D3DRS_ZENABLE, &dwOldZEnable);
    m_pGraphicDev->GetRenderState(D3DRS_ZWRITEENABLE, &dwOldZWrite);
    m_pGraphicDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwOldAlphaBlend);
    m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dwOldCull);

    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    _matrix matView, matProj;
    D3DXMatrixIdentity(&matView);
    D3DXMatrixOrthoLH(&matProj, (float)WINCX, (float)WINCY, 0.f, 1.f);

    // m_vInfo[INFO_POS]를 "화면 좌상단 기준 픽셀 좌표"로 사용한다고 가정
    // (x, y) = UI의 좌상단 위치, scale.x/scale.y = UI 폭/높이
    _vec3 vPos = m_pTransformCom->m_vInfo[INFO_POS];
    _vec3 vScale = m_pTransformCom->m_vScale;

    _matrix matScale, matTrans, matWorld;
    D3DXMatrixScaling(&matScale, vScale.x, vScale.y, 1.f);

    const _float fWorldX = vPos.x - (WINCX * 0.5f) + (vScale.x * 0.5f);
    const _float fWorldY = (WINCY * 0.5f) - vPos.y - (vScale.y * 0.5f);
    D3DXMatrixTranslation(&matTrans, fWorldX, fWorldY, 0.f);

    matWorld = matScale * matTrans;

    m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
    m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

    if (nullptr != m_pTextureCom)
        m_pTextureCom->Set_Texture((_uint)m_fFrame);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matOldWorld);
    m_pGraphicDev->SetTransform(D3DTS_VIEW, &matOldView);
    m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOldProj);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, dwOldCull);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwOldAlphaBlend);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, dwOldZWrite);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, dwOldZEnable);
}

void CUI::Set_Pos(const _vec2& vPos)
{
    m_vPos = vPos;
    if (nullptr != m_pTransformCom)
		m_pTransformCom->Set_Pos(vPos.x, vPos.y, 0.f);
}

void CUI::Set_Size(const _vec2& vSize)
{
    m_vSize = vSize;
    if (nullptr != m_pTransformCom)
        m_pTransformCom->Set_Scale(m_vSize.x, m_vSize.y, 1.f);
}

void CUI::Set_Texture(const _uint& iIndex)
{
    m_fFrame = (_float)iIndex;
}

HRESULT CUI::Add_Component()
{
    CComponent* pComponent = nullptr;

    // RcTex
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Texture (태그가 있을 때만 생성)
    if (false == m_wstrTextureTag.empty())
    {
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(m_wstrTextureTag.c_str()));
        if (nullptr == pComponent)
            return E_FAIL;

        m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
    }

    return S_OK;
}

CUI* CUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CUI* pGameUI = new CUI(pGraphicDev);

    if (FAILED(pGameUI->Ready_GameObject()))
    {
        Safe_Release(pGameUI);
        MSG_BOX("CUI Create Failed");
        return nullptr;
    }

    return pGameUI;
}

CUI* CUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrTextureTag)
{
    CUI* pGameUI = new CUI(pGraphicDev);
    pGameUI->m_wstrTextureTag = wstrTextureTag;

    if (FAILED(pGameUI->Ready_GameObject()))
    {
        Safe_Release(pGameUI);
        MSG_BOX("CUI Create Failed");
        return nullptr;
    }

    return pGameUI;
}

void CUI::Free()
{
    CGameObject::Free();
}
