#include "pch.h"
#include "CAtmosphereVeil.h"
#include "CTransform.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "Client_Enum.h"
#include "CVeilSphere.h"
#include "CLayerContext.h"

CAtmosphereVeil::CAtmosphereVeil(LPDIRECT3DDEVICE9 pGraphicDev, const TVeilDesc& tDesc)
    : CGameObject(pGraphicDev), m_tDesc(tDesc)
{
}

CAtmosphereVeil::~CAtmosphereVeil()
{
}

HRESULT CAtmosphereVeil::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    
    SpawnSphere();

    return S_OK;
}

_int CAtmosphereVeil::Update_GameObject(_float fTimeDelta)
{
    if (!Get_IsActive()) return S_OK;

    if (!m_pTarget)
    {
        m_pTarget = CManagement::GetInstance()->GetCurrentScene()->Get_GameObject(L"GameLogic_Layer", L"Player");
    }

    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    return iExit;
}

void CAtmosphereVeil::LateUpdate_GameObject(_float fTimeDelta)
{
    if (!Get_IsActive()) return;

    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CAtmosphereVeil::Render_GameObject()
{
}

CTransform* CAtmosphereVeil::GetTargetTransform()
{
    if (!m_pTarget) return nullptr;

    return static_cast<CTransform*>(m_pTarget->Get_Component(ID_DYNAMIC, L"Com_Transform"));
}

void CAtmosphereVeil::Set_IsActive(_bool bIsActive)
{
    m_bIsActive = bIsActive;
    for (auto p : m_vecSphere)
    {
        p->Set_IsActive(bIsActive);
    }
}

HRESULT CAtmosphereVeil::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Buffer
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Sphere_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    unordered_map<EColorTexture, wstring> mapName = {
        {EColorTexture::BLACK, L"Black"},
        {EColorTexture::ORANGE, L"Orange"},
        {EColorTexture::PINK, L"Pink"},
        {EColorTexture::RED, L"Red"},
        {EColorTexture::YELLOW, L"Yellow"},
        {EColorTexture::WHITE, L"White"},
        {EColorTexture::SAND, L"Sand"},
    };
    wstring wstrTextureName = L"Proto_" + mapName.at(m_tDesc.eColor) + L"_Texture";
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(wstrTextureName.c_str()));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CAtmosphereVeil::SpawnSphere()
{
    EColorTexture eColor = m_tDesc.eColor;

    assert(m_tDesc.vecOpacity.size() == m_tDesc.vecRadius.size());
    int iSize = (int)m_tDesc.vecOpacity.size();

    for (int i = 0; i < iSize; ++i)
    {
        CVeilSphere* pSphere = CVeilSphere::Create(m_pGraphicDev);
        if (pSphere)
        {
            CLayerContext::GetLayer()->Add_GameObject(L"VeilSphere_" + to_wstring(i), pSphere);
            pSphere->SetRadius(m_tDesc.vecRadius.at(i));
            pSphere->SetOpacity(m_tDesc.vecOpacity.at(i));
            pSphere->SetParent(this);
            m_vecSphere.push_back(pSphere);
        }
    }
}

CAtmosphereVeil* CAtmosphereVeil::Create(LPDIRECT3DDEVICE9 pGraphicDev, const TVeilDesc& tDesc)
{
    CAtmosphereVeil* pDoor = new CAtmosphereVeil(pGraphicDev, tDesc);

    if (FAILED(pDoor->Ready_GameObject()))
    {
        Safe_Release(pDoor);
        MSG_BOX("CAtmosphereVeil Create Failed");
        return nullptr;
    }

    return pDoor;
}

void CAtmosphereVeil::Free()
{
    CGameObject::Free();
}
