#include "pch.h"
#include "CGemBox.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CRcTex.h"
#include "CSphereCollider.h"
#include "CCollisionMgr.h"
#include "CManagement.h"
#include "CStage.h"
#include "CGameStatus.h"
#include "CUI.h"

CGemBox::CGemBox(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

HRESULT CGemBox::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(0.5f, 0.75f, 0.5f);
    m_pColliderCom->Set_Owner(this);
    m_pColliderCom->Set_IsTrigger(true);
    m_pColliderCom->Set_Radius(0.75f);
    return S_OK;
}

_int CGemBox::Update_GameObject(_float fTimeDelta)
{
    if (!m_bStart)
    {
        m_bStart = true;
        _vec3 vPos;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        vPos.y += m_pTransformCom->Get_Scale().y;
        m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z - 0.45f);
    }

    m_fFrameDelta = max(0.f, fTimeDelta);
    m_bDepositedThisFrame = false;
    if (m_iRemainingGems > 0)
        m_fFrame = fmodf(m_fFrame + m_fFrameDelta * 6.f, 3.f);

    const _int iExit = CGameObject::Update_GameObject(fTimeDelta);
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);
    CCollisionMgr::GetInstance()->Add_Collider(COLL_ITEM, m_pColliderCom);
    return iExit;
}

void CGemBox::OnCollisionEnter(COLLINFO info)
{
    Receive_Gem(info);
}

void CGemBox::OnCollisionStay(COLLINFO info)
{ 
    Receive_Gem(info);
}

void CGemBox::OnCollisionExit(COLLINFO info)
{
    if (info.iOtherID == COLL_PLAYER)
        m_fDepositTime = 0.f;
}

void CGemBox::Receive_Gem(COLLINFO info)
{
    if (info.iOtherID != COLL_PLAYER || m_iRemainingGems <= 0 || m_bDepositedThisFrame)
        return;

    m_bDepositedThisFrame = true;

    auto* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
    if (!pStage || !pStage->GetStatus())
        return;

    auto* pStatus = pStage->GetStatus();
    if (pStatus->GetGemCount() <= 0)
    {
        m_fDepositTime = 0.f;
        return;
    }

    m_fDepositTime += m_fFrameDelta;

    _bool bChanged = false;
    while (m_fDepositTime >= m_fDepositInterval && m_iRemainingGems > 0 && pStatus->GetGemCount() > 0)
    {
        m_fDepositTime -= m_fDepositInterval;
        pStatus->UpdateGem(-1);
        --m_iRemainingGems;
        bChanged = true;
    }

    if (m_iRemainingGems == 0 || pStatus->GetGemCount() == 0)
        m_fDepositTime = 0.f;
    if (bChanged)
        Update_GemHUD();
}

void CGemBox::Update_GemHUD()
{
    auto* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
    if (!pStage || !pStage->GetStatus())
        return;

    const _int iCount = max(0, min(999, pStage->GetStatus()->GetGemCount()));
    _int iDivisor = 100;
    for (_int i = 0; i < 3; ++i, iDivisor /= 10)
    {
        const wstring tag = L"GemNum_" + to_wstring(i);
        auto* pUI = dynamic_cast<CUI*>(CManagement::GetInstance()->Get_GameObject(L"UI_Layer", tag.c_str()));
        if (pUI)
            pUI->Set_Texture((iCount / iDivisor) % 10);
    }
}

void CGemBox::Render_GameObject()
{
    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);

    DWORD dwCull, dwMin, dwMag;
    m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
    m_pGraphicDev->GetSamplerState(0, D3DSAMP_MINFILTER, &dwMin);
    m_pGraphicDev->GetSamplerState(0, D3DSAMP_MAGFILTER, &dwMag);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

    auto DrawSprite = [&](CTexture* pTexture, _uint iFrame, const _vec3& pos, _float x, _float y)
    {
        _matrix scale, translation, world;
        D3DXMatrixScaling(&scale, -x, y, 1.f);
        D3DXMatrixTranslation(&translation, pos.x, pos.y, pos.z);
        world = scale * translation;
        m_pGraphicDev->SetTransform(D3DTS_WORLD, &world);
        pTexture->Set_Texture(iFrame);
        m_pBufferCom->Render_Buffer();
    };

    const _vec3 vScale = m_pTransformCom->Get_Scale();
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture(m_iRemainingGems == 0 ? 3u : (_uint)m_fFrame);
    m_pBufferCom->Render_Buffer();
    const _vec3 vLabel = vPos + _vec3(0.f, vScale.y + 0.3f, 0.f);
    const wstring strCount = to_wstring(m_iRemainingGems);
    const _float fWidth = 0.32f + 0.16f * static_cast<_float>(strCount.size());

    DrawSprite(m_pGemTextureCom, 0, vLabel + _vec3(fWidth * 0.5f - 0.12f, 0.f, 0.f), 0.12f, 0.12f);
    for (size_t i = 0; i < strCount.size(); ++i)
    {
        const _float x = -fWidth * 0.5f + 0.32f + 0.08f + static_cast<_float>(i) * 0.16f;
        DrawSprite(m_pNumberTextureCom, strCount[i] - L'0', vLabel + _vec3(-x, 0.f, 0.f), 0.12f, 0.16f);
    }
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, dwCull);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, dwMin);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, dwMag);
}

HRESULT CGemBox::Add_Component()
{
    m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    if (!m_pTransformCom)
        return E_FAIL;
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", m_pTransformCom });

    m_pColliderCom = dynamic_cast<CSphereCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SphereCollider"));
    if (!m_pColliderCom)
        return E_FAIL;
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", m_pColliderCom });

    m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));
    if (!m_pBufferCom)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", m_pBufferCom });

    m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_GemBox_Texture"));
    if (!m_pTextureCom)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

    m_pGemTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Item_Gem_Texture"));
    if (!m_pGemTextureCom)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_GemTexture", m_pGemTextureCom });

    m_pNumberTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_NumberTexture"));
    if (!m_pNumberTextureCom)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_NumberTexture", m_pNumberTextureCom });
    return S_OK;
}

CGemBox* CGemBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    auto* pObject = new CGemBox(pGraphicDev);
    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        return nullptr;
    }
    return pObject;
}

void CGemBox::Free() { CGameObject::Free(); }
