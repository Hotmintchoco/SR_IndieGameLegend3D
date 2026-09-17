#include "pch.h"
#include "CWall.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CBoxCollider.h"
#include "CTransform.h"
#include "CCollisionMgr.h"
#include "CSphereCollider.h"
#include "CLayerContext.h"
#include "CRoomLayer.h"
#include "Client_Struct.h"
#include <algorithm>
#include <cfloat>
#include <ctime>

CWall::CWall(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CWall::CWall(LPDIRECT3DDEVICE9 pGraphicDev, EWallDir eDir, bool bHasDoor)
    : CGameObject(pGraphicDev), m_eDir(eDir), m_bHasDoor(bHasDoor)
{
}


CWall::~CWall()
{
}

HRESULT CWall::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    InitializeCollider();

    CRoomLayer* pLayer = static_cast<CRoomLayer*>(CLayerContext::GetLayer());
    if (pLayer)
    {
        pLayer->m_OnRoomEvent.AddBinding(GetToken(), [this](const TRoomEventCtx& t) { OnRoomEvent(t); });    }

    /* Set Initial Position */
    if (m_eDir == EWallDir::WEST || m_eDir == EWallDir::SOUTH)
    {
        m_pTransformCom->Rotation(ROT_Y, 180.f);
    }

	__super::Ready_GameObject();

    return S_OK;
}

void CWall::InitializeCollider()
{
    // -- Collider Initialization -- 

    // North Wall Collider
    _float fExtentsX(0.f), fExtentsY(2.f), fExtentsZ(0.f);
    _float fDiffX(0.f), fDiffY(1.f), fDiffZ(0.f);

    switch (m_eDir)
    {
    case EWallDir::EAST:
        fExtentsX = 0.5f;
        fExtentsZ = 3.5f;
        fDiffX = 7.f;
        break;
    case EWallDir::SOUTH:
        fExtentsX = 3.5f;
        fExtentsZ = 0.5f;
        fDiffZ = -6.f;
        break;
    case EWallDir::WEST:
        fExtentsX = 0.5f;
        fExtentsZ = 3.5f;
        fDiffX = -7.f;
        break;
    case EWallDir::NORTH:
        fExtentsX = 3.5f;
        fExtentsZ = 0.5f;
        fDiffZ = 6.f;
        break;
    }

    if (m_bHasDoor)
    {
        if (m_eDir == EWallDir::EAST || m_eDir == EWallDir::WEST)
            fDiffZ = 4.5f;
        else
            fDiffX = 4.5f;
    }
    else
    {
        if (m_eDir == EWallDir::EAST || m_eDir == EWallDir::WEST)
            fDiffZ = 3.f;
        else
            fDiffX = 3.f;
    }

    CBoxCollider* pBoxCollider = static_cast<CBoxCollider*>(m_pColliderCom[0]);
    pBoxCollider->Set_Extents(fExtentsX, fExtentsY, fExtentsZ);
    pBoxCollider->Set_DiffPos({ fDiffX, fDiffY, fDiffZ });

    if (m_eDir == EWallDir::EAST || m_eDir == EWallDir::WEST)
        fDiffZ = -fDiffZ;
    else
        fDiffX = -fDiffX;

    pBoxCollider = static_cast<CBoxCollider*>(m_pColliderCom[1]);
    pBoxCollider->Set_Extents(fExtentsX, fExtentsY, fExtentsZ);
    pBoxCollider->Set_DiffPos({ fDiffX, fDiffY, fDiffZ });

    for (int i = 0; i < 2; ++i)
    {
        if (m_pColliderCom[i])
            m_pColliderCom[i]->Set_CollisionID(COLL_OBSTACLE);
	}
    // -- Collider Initialization -- 
}

void CWall::OnRoomEvent(const TRoomEventCtx& t)
{
    if (!m_bHasDoor) return;

    switch (t.eType)
    {
    case ERoomEventType::ROOM_BEGIN:
        BlockDoor(true);
        break;
    case ERoomEventType::ROOM_CLEAR:
        BlockDoor(false);
        break;
    default:
        break;
    }
}

void CWall::BlockDoor(bool bBlock)
{
    // -- Collider Initialization -- 

    // North Wall Collider
    _float fExtentsX(0.f), fExtentsY(2.f), fExtentsZ(0.f);
    _float fDiffX(0.f), fDiffY(1.f), fDiffZ(0.f);

    switch (m_eDir)
    {
    case EWallDir::EAST:
        fExtentsX = 0.5f;
        fExtentsZ = 3.5f;
        fDiffX = 7.f;
        break;
    case EWallDir::SOUTH:
        fExtentsX = 3.5f;
        fExtentsZ = 0.5f;
        fDiffZ = -6.f;
        break;
    case EWallDir::WEST:
        fExtentsX = 0.5f;
        fExtentsZ = 3.5f;
        fDiffX = -7.f;
        break;
    case EWallDir::NORTH:
        fExtentsX = 3.5f;
        fExtentsZ = 0.5f;
        fDiffZ = 6.f;
        break;
    }

    if (!bBlock)
    {
        if (m_eDir == EWallDir::EAST || m_eDir == EWallDir::WEST)
            fDiffZ = 4.5f;
        else
            fDiffX = 4.5f;
    }
    else
    {
        if (m_eDir == EWallDir::EAST || m_eDir == EWallDir::WEST)
            fDiffZ = 3.f;
        else
            fDiffX = 3.f;
    }

    CBoxCollider* pBoxCollider = static_cast<CBoxCollider*>(m_pColliderCom[0]);
    pBoxCollider->Set_Extents(fExtentsX, fExtentsY, fExtentsZ);
    pBoxCollider->Set_DiffPos({ fDiffX, fDiffY, fDiffZ });

    if (m_eDir == EWallDir::EAST || m_eDir == EWallDir::WEST)
        fDiffZ = -fDiffZ;
    else
        fDiffX = -fDiffX;

    pBoxCollider = static_cast<CBoxCollider*>(m_pColliderCom[1]);
    pBoxCollider->Set_Extents(fExtentsX, fExtentsY, fExtentsZ);
    pBoxCollider->Set_DiffPos({ fDiffX, fDiffY, fDiffZ });

    // -- Collider Initialization -- 
}

_int CWall::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CWall::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    // 충돌 처리
    for (int i = 0; i < 2; ++i)
        CCollisionMgr::GetInstance()->Add_Collider(COLL_OBSTACLE, m_pColliderCom[i]);
}

void CWall::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();

#ifdef _DEBUG
    for (int i = 0; i < 2; ++i)
    {
        CBoxCollider* pBoxCollider = dynamic_cast<CBoxCollider*>(m_pColliderCom[i]);
        if (nullptr != pBoxCollider)
            pBoxCollider->Render_DebugCube();
	}
#endif
}

void CWall::OnCollisionEnter(CGameObject* pOther)
{
    
}

void CWall::OnCollisionStay(CGameObject* pOther)
{
    if (nullptr == pOther)
        return;

    for (int i = 0; i < 2; ++i)
    {
        if (nullptr == m_pColliderCom[i])
            continue;

        Obstacle_Collision(pOther, m_pColliderCom[i]);
    }
}

HRESULT CWall::Add_Component()
{
    CComponent* pComponent = nullptr;

    wstring wstrBufferName, wstrTextureName, wstrDir, wstrDoor;

    if (m_bHasDoor)
    {
        wstrDoor = L"";
    }
    else
    {
        wstrDoor = L"No";
    }

    switch (m_eDir)
    {
    case EWallDir::EAST:
    case EWallDir::WEST:
        wstrDir = L"EW";
        break;
    case EWallDir::NORTH:
    case EWallDir::SOUTH:
        wstrDir = L"NS";
        break;
    default:
        assert(0);
        break;
    }

    wstrBufferName = L"Proto_Wall_" + wstrDir + L"_" + wstrDoor + L"Door_Vertex";
    wstrTextureName = L"Proto_Wall_" + wstrDir + L"_" + wstrDoor + L"Door_Texture";

    // TerrainTex
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(wstrBufferName.c_str()));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(wstrTextureName.c_str()));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Collider
    static const _tchar* sColliderTags[2] =
    {
        L"Com_Collider0",
        L"Com_Collider1",
    };

    for (int i = 0; i < 2; ++i)
    {
        pComponent = m_pColliderCom[i] = dynamic_cast<CCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BoxCollider"));
        if (nullptr == pComponent)
            return E_FAIL;

        m_mapComponent[ID_DYNAMIC].insert({ sColliderTags[i], pComponent });
    }

    return S_OK;
}

CWall* CWall::Create(LPDIRECT3DDEVICE9 pGraphicDev, EWallDir eDir, bool bHasDoor)
{
    CWall* pWall = new CWall(pGraphicDev, eDir, bHasDoor);

    if (FAILED(pWall->Ready_GameObject()))
    {
        Safe_Release(pWall);
        MSG_BOX("CWall Create Failed");
        return nullptr;
    }

    return pWall;
}

void CWall::Free()
{
    CGameObject::Free();
}
