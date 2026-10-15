#include "pch.h"
#include "CColorArea.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "CLiminalFrame.h"

CColorArea::CColorArea(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CColorArea::~CColorArea()
{
}

HRESULT CColorArea::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(2.f, 2.f, 2.f);
    m_pBufferCom->Set_Owner(this);
    m_pColliderCom->Set_Owner(this);
    m_pColliderCom->Set_Extents(_vec3{1.f, 1.f, 1.f});

    return S_OK;
}

_int CColorArea::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    if (m_bIsTimerActive)
    {
        m_fTimeAfterStart += fTimeDelta;
        if (m_fTimeAfterStart >= m_fRequiredTime)
        {
            m_OnColorAreaSatisfied.Broadcast(m_eType);
            m_bIsTimerActive = false;
            m_fTimeAfterStart = 0.f;
            
            m_bSatisfied = true;
            m_pColliderCom->Set_IsActive(false);

            m_pFrame->Capture();
        }
    }

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
    CCollisionMgr::GetInstance()->Add_Collider(COLL_ROOMLOGIC, m_pColliderCom);

    _vec3 vPos = m_pTransformCom->Get_Info_Value(INFO_POS);
    Compute_ViewZ(&vPos);

    return iExit;
}

void CColorArea::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CColorArea::Render_GameObject()
{
    if (nullptr == m_pTextureCom || m_bSatisfied)
        return;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(50);
    m_pBufferCom->Render_Buffer();
}

void CColorArea::OnCollisionEnter(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    switch (iOtherID)
    {
    case COLLISIONID::COLL_ROOMLOGIC:
    {
        if (CLiminalFrame* pFrame = dynamic_cast<CLiminalFrame*>(pOtherCol->Get_Owner()))
        {
            EColorType eType = pFrame->GetFrameColor();
            if (m_eType == eType)
            {
                m_pFrame = pFrame;
                StartTimer();
            }
        }
        break;
    }
    default:
        break;
    }
}

void CColorArea::OnCollisionStay(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    switch (iOtherID)
    {
    case COLLISIONID::COLL_PLAYER:
    {
        Obstacle_Collision(eCollInfo.pOtherCollider, m_pColliderCom);
        break;
    }
    default:
        break;
    }
}

void CColorArea::OnCollisionExit(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    switch (iOtherID)
    {
    case COLLISIONID::COLL_ROOMLOGIC:
    {
        if (CLiminalFrame* pFrame = dynamic_cast<CLiminalFrame*>(pOtherCol->Get_Owner()))
        {
            EColorType eType = pFrame->GetFrameColor();
            if (m_eType == eType)
            {
                m_pFrame = nullptr;
                StopTimer();
            }
        }
        break;
    }
    default:
        break;
    }
}

void CColorArea::SetColor(EColorType eType)
{
    m_eType = eType;

    auto iter = s_mapColor.find(eType);
    if (iter == s_mapColor.end())
        return;

    wstring wstrProtoTag = L"Proto_" + iter->second + L"_Texture";

    CTexture* pTexture = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(wstrProtoTag.c_str()));

    if (nullptr == pTexture)
    {
        MSG_BOX("CColorArea::SetColor Failed");
        return;
    }

    /* Release Texture if already exists */
    if (nullptr != m_pTextureCom)
    {
        auto& mapCom = m_mapComponent[ID_DYNAMIC];
        for (auto iterCom = mapCom.begin(); iterCom != mapCom.end(); ++iterCom)
        {
            if (iterCom->second == m_pTextureCom)
            {
                Safe_Release(iterCom->second);
                mapCom.erase(iterCom);
                break;
            }
        }
    }

    m_pTextureCom = pTexture;
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pTexture });

}

HRESULT CColorArea::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Buffer
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Cube_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CBoxCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BoxCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    return S_OK;
}

void CColorArea::StartTimer()
{
    m_bIsTimerActive = true;
    m_fTimeAfterStart = 0.f;
}

void CColorArea::StopTimer()
{
    m_bIsTimerActive = false;
    m_fTimeAfterStart = 0.f;
}

CColorArea* CColorArea::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CColorArea* pObject = new CColorArea(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CColorArea Create Failed");
        return nullptr;
    }

    return pObject;
}

void CColorArea::Free()
{
    CGameObject::Free();
}