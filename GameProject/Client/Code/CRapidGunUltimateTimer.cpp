#include "pch.h"
#include "CRapidGunUltimateTimer.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CRapidGunUltimateTimer::CRapidGunUltimateTimer(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CRapidGunUltimateTimer::~CRapidGunUltimateTimer()
{
}

HRESULT CRapidGunUltimateTimer::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    SetSize(64, 64);

    m_pTransform->Set_Pos(_vec3{ 0.f, 0.f, 0.1f });

    m_iTextureIndex = 0;
    m_iTextureFrameCount = m_pTexture->GetCount();

    return S_OK;
}

_int CRapidGunUltimateTimer::Update_GameObject(_float fTimeDelta)
{
    if (!Get_IsActive()) return S_OK;

    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return iExit;
}

void CRapidGunUltimateTimer::LateUpdate_GameObject(_float fTimeDelta)
{
    if (!Get_IsActive()) return;

    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CRapidGunUltimateTimer::Render_GameObject()
{
    if (!Get_IsActive()) return;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransform->Get_World());
    m_pTexture->Set_Texture(m_iTextureIndex);
    m_pBuffer->Render_Buffer();
}

void CRapidGunUltimateTimer::SetSize(int iX, int iY)
{
    m_pTransform->Set_Scale(_vec3{ iX * 0.5f, iY * 0.5f, 1.f });
}

void CRapidGunUltimateTimer::SetTimeRatio(float fRatio)
{
    fRatio = clamp(fRatio, 0.f, 1.f);

    m_iTextureIndex = static_cast<int>(fRatio * m_iTextureFrameCount);

    if (m_iTextureIndex >= m_iTextureFrameCount)
        m_iTextureIndex = m_iTextureFrameCount - 1;
}

HRESULT CRapidGunUltimateTimer::Add_Component()
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
    pComponent = m_pTexture = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Ultimate_RapidGun_Timer_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CRapidGunUltimateTimer* CRapidGunUltimateTimer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CRapidGunUltimateTimer* pTimer = new CRapidGunUltimateTimer(pGraphicDev);

    if (FAILED(pTimer->Ready_GameObject()))
    {
        Safe_Release(pTimer);
        MSG_BOX("CRapidGunUltimateTimer Create Failed");
        return nullptr;
    }

    return pTimer;
}

void CRapidGunUltimateTimer::Free()
{
    CGameObject::Free();
}