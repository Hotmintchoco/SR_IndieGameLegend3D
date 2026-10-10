#include "pch.h"
#include "CRapidGunUltimateScope.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CRapidGunUltimateScope::CRapidGunUltimateScope(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CRapidGunUltimateScope::~CRapidGunUltimateScope()
{
}

HRESULT CRapidGunUltimateScope::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    SetSize(static_cast<int>(768 * 1.5f), static_cast<int>(324 * 1.5f));

    m_pTransform->Set_Pos(_vec3{ 0.f, 0.f, 0.1f });

    m_bPlayingReverse = false;
    m_fTimeAfterPlay = 0.f;
    m_iTextureIndex = 0;
    m_iTextureFrameCount = m_pTexture->GetCount();

    return S_OK;
}

_int CRapidGunUltimateScope::Update_GameObject(_float fTimeDelta)
{
    if (!Get_IsActive()) return S_OK;

    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    UpdateAnimation(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return iExit;
}

void CRapidGunUltimateScope::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CRapidGunUltimateScope::Render_GameObject()
{
    if (!Get_IsActive()) return;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransform->Get_World());
    m_pTexture->Set_Texture(m_iTextureIndex);
    m_pBuffer->Render_Buffer();
}

void CRapidGunUltimateScope::SetSize(int iX, int iY)
{
    // RcTex 정점이 -1 ~ 1 이므로 픽셀 크기의 절반을 스케일로 사용
    m_pTransform->Set_Scale(_vec3{ iX * 0.5f, iY * 0.5f, 1.f });
}

void CRapidGunUltimateScope::Begin()
{
    m_bIsActive = true;
    m_bPlayingReverse = false;
    m_fTimeAfterPlay = 0.f;
    m_iTextureIndex = 0;
}

void CRapidGunUltimateScope::End()
{
    if (!Get_IsActive()) return;

    m_bPlayingReverse = true;
    int iLastIndex = m_iTextureFrameCount - 1;
    m_fTimeAfterPlay = (iLastIndex - m_iTextureIndex) * m_fFrameInterval;
}

void CRapidGunUltimateScope::UpdateAnimation(float fTimeDelta)
{
    if (m_iTextureFrameCount <= 0) return;

    m_fTimeAfterPlay += fTimeDelta;

    int iLastIndex = m_iTextureFrameCount - 1;
    int iStep = static_cast<int>(m_fTimeAfterPlay / m_fFrameInterval);

    if (!m_bPlayingReverse)
    {
        m_iTextureIndex = min(iStep, iLastIndex);
    }
    else
    {
        int iIndex = iLastIndex - iStep;

        if (iIndex < 0)
        {
            m_iTextureIndex = 0;
            m_bIsActive = false;
            m_bPlayingReverse = false;
            return;
        }

        m_iTextureIndex = iIndex;
    }
}

HRESULT CRapidGunUltimateScope::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransform = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Buffer
    pComponent = m_pBuffer = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTexture = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Ultimate_RapidGun_Scope_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CRapidGunUltimateScope* CRapidGunUltimateScope::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CRapidGunUltimateScope* pScope = new CRapidGunUltimateScope(pGraphicDev);

    if (FAILED(pScope->Ready_GameObject()))
    {
        Safe_Release(pScope);
        MSG_BOX("CRapidGunUltimateScope Create Failed");
        return nullptr;
    }

    return pScope;
}

void CRapidGunUltimateScope::Free()
{
    CGameObject::Free();
}