#include "pch.h"
#include "CDoor.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CRoomLayer.h"
#include "CLayerContext.h"
#include "Client_Struct.h"

CDoor::CDoor(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CDoor::~CDoor()
{
}

HRESULT CDoor::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(0.5f, 0.75f, 1.f);

    CRoomLayer* pLayer = static_cast<CRoomLayer*>(CLayerContext::GetLayer());
    if (pLayer)
    {
        pLayer->m_OnRoomEvent.AddBinding(GetToken(), [this](const TRoomEventCtx& t) { OnRoomEvent(t); });
    }

    Open();

    return S_OK;
}

_int CDoor::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);

    if (m_bOnAnimation)
    {
        m_fSingleFrameAccTime += fTimeDelta;
        if (m_fSingleFrameAccTime > m_fFrameInterval)
        {
            m_fSingleFrameAccTime -= m_fFrameInterval;
            m_iTextureIdx += 1 * m_iPlayDirection;
        }

        if (m_iPlayDirection == -1 && m_iTextureIdx <= 0)
        {
            m_bOnAnimation = false;
            m_iTextureIdx = 0;
        }
        else if (m_iPlayDirection == 1 && m_iTextureIdx >= m_iFrameCnt)
        {
            m_bOnAnimation = false;
            m_iTextureIdx = m_iFrameCnt - 1;
        }
    }

    return iExit;
}

void CDoor::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CDoor::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(m_iTextureIdx);

    if (m_iTextureIdx != 0)
    {
        m_pBufferCom->Render_Buffer();
    }
}

HRESULT CDoor::Add_Component()
{
    CComponent* pComponent = nullptr;

    // TerrainTex
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Door_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });


    return S_OK;
}

void CDoor::OnRoomEvent(const TRoomEventCtx& t)
{
    switch (t.eType)
    {
    case ERoomEventType::ROOM_BEGIN:
        Close();
        break;
    case ERoomEventType::ROOM_CLEAR:
    case ERoomEventType::RESET_ROOM:
        Open();
        break;
    default:
        break;
    }
}

void CDoor::Open()
{
    m_bOnAnimation = true;
    m_iPlayDirection = -1;
    m_fSingleFrameAccTime = 0.f;
}

void CDoor::Close()
{
    m_bOnAnimation = true;
    m_iPlayDirection = 1;
    m_fSingleFrameAccTime = 0.f;
}

CDoor* CDoor::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CDoor* pDoor = new CDoor(pGraphicDev);

    if (FAILED(pDoor->Ready_GameObject()))
    {
        Safe_Release(pDoor);
        MSG_BOX("CDoor Create Failed");
        return nullptr;
    }

    return pDoor;
}

void CDoor::Free()
{
    CGameObject::Free();
}
