#include "pch.h"
#include "CWall.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CBoxCollider.h"
#include "CTransform.h"
#include "CCollisionMgr.h"
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

    // -- Collider Initialization -- 
	// North Wall Collider
    CBoxCollider* pBoxCollider = static_cast<CBoxCollider*>(m_pColliderCom[0]);
    pBoxCollider->Set_Extents(6.5f, 2.f, 0.5f);
    pBoxCollider->Set_DiffPos({ 0.f, 0.f, 6.f });

	// South Wall Collider
    pBoxCollider = static_cast<CBoxCollider*>(m_pColliderCom[1]);
	pBoxCollider->Set_Extents(6.5f, 2.f, 0.5f);
	pBoxCollider->Set_DiffPos({ 0.f, 0.f, -6.f });

	// East Wall Collider
	pBoxCollider = static_cast<CBoxCollider*>(m_pColliderCom[2]);
	pBoxCollider->Set_Extents(0.5f, 2.f, 6.5f);
	pBoxCollider->Set_DiffPos({ 7.f, 0.f, 0.f });

	// West Wall Collider
	pBoxCollider = static_cast<CBoxCollider*>(m_pColliderCom[3]);
	pBoxCollider->Set_Extents(0.5f, 2.f, 6.5f);
	pBoxCollider->Set_DiffPos({ -7.f, 0.f, 0.f });

    // -- Collider Initialization -- 

    /* Set Initial Position */
    if (m_eDir == EWallDir::WEST || m_eDir == EWallDir::SOUTH)
    {
        m_pTransformCom->Rotation(ROT_Y, 180.f);
    }

	__super::Ready_GameObject();

    return S_OK;
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
	for (int i = 0; i < 4; ++i)
	    CCollisionMgr::GetInstance()->Add_Collider(COLL_WALL, m_pColliderCom[i]);
}

void CWall::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();

#ifdef _DEBUG
    for(int i = 0; i < 4; ++i)
	{
		CBoxCollider* pBoxCollider = dynamic_cast<CBoxCollider*>(m_pColliderCom[i]);
        if (nullptr != pBoxCollider)
			pBoxCollider->Render_DebugCube();
    }
#endif
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
    static const _tchar* sColliderTags[4] =
    {
        L"Com_Collider_0",
        L"Com_Collider_1",
        L"Com_Collider_2",
        L"Com_Collider_3"
    };

    for (int i = 0; i < 4; ++i)
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
