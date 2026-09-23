#include "pch.h"
#include "CFrustumExplodeEffect.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"

CFrustumExplodeEffect::CFrustumExplodeEffect(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEffect(pGraphicDev)
{
}


CFrustumExplodeEffect::~CFrustumExplodeEffect()
{
}

HRESULT CFrustumExplodeEffect::Ready_GameObject()
{
    CEffect::Ready_GameObject();

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_iTotalFrameCount = static_cast<CTexture*>(m_pTextureCom)->GetCount();

    return S_OK;
}

_int CFrustumExplodeEffect::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CEffect::Update_GameObject(fTimeDelta);

    m_fSingleFrameAccTime += fTimeDelta;
    if (m_fSingleFrameAccTime >= m_fFrameInterval)
    {
        m_fSingleFrameAccTime -= m_fFrameInterval;
        ++m_iCurrentFrame;

        if (m_iCurrentFrame == m_iTotalFrameCount)
        {
            Set_Dead(true);
        }
    }


    return iExit;
}

void CFrustumExplodeEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CEffect::LateUpdate_GameObject(fTimeDelta);

    BillBoard();
}

void CFrustumExplodeEffect::BillBoard()
{
    /* ºôº¸µå */
    _vec3 vPlayerPos, vEffectPos;
    CTransform* pPlayerTransform = static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
    pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
    m_pTransformCom->Get_Info(INFO_POS, &vEffectPos);
    _vec3 vDist = vEffectPos - vPlayerPos;

    float fYaw = atan2f(vDist.x, vDist.z);

    m_pTransformCom->Set_Rotation_Raw(_vec3{ 0.f, D3DXToDegree(fYaw), 0.f });
}

void CFrustumExplodeEffect::Render_GameObject()
{
    if (Is_Dead()) return;

    CEffect::Render_GameObject();

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture(m_iCurrentFrame);

    m_pBufferCom->Render_Buffer();
}

HRESULT CFrustumExplodeEffect::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_FrustumExplode_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // RcTex
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });


    return S_OK;
}


CFrustumExplodeEffect* CFrustumExplodeEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CFrustumExplodeEffect* pFrustumExplodeEffect = new CFrustumExplodeEffect(pGraphicDev);

    if (FAILED(pFrustumExplodeEffect->Ready_GameObject()))
    {
        Safe_Release(pFrustumExplodeEffect);
        MSG_BOX("CFrustumExplodeEffect Create Failed");
        return nullptr;
    }

    return pFrustumExplodeEffect;
}


CFrustumExplodeEffect* CFrustumExplodeEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale)
{
    CFrustumExplodeEffect* pFrustumExplodeEffect = new CFrustumExplodeEffect(pGraphicDev);

    if (FAILED(pFrustumExplodeEffect->Ready_GameObject()))
    {
        Safe_Release(pFrustumExplodeEffect);
        MSG_BOX("CFrustumExplodeEffect Create Failed");
        return nullptr;
    }

    pFrustumExplodeEffect->Set_Pos(vPos);
    pFrustumExplodeEffect->Set_Scale(vScale);

    return pFrustumExplodeEffect;
}


void CFrustumExplodeEffect::Free()
{
    CEffect::Free();
}
