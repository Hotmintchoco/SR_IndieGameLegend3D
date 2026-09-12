#include "pch.h"
#include "CRoom.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CTile.h"
#include "CWall.h"
#include "CFog.h"
#include "CManagement.h"
#include "CRoomLoadingMgr.h"
#include "CLayer.h"
#include "CMonster.h"
#include "CFrustum.h"

CRoom::CRoom(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CRoom::CRoom(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex)
    : CGameObject(pGraphicDev), m_iIndex(iIndex)
{
}

CRoom::~CRoom()
{
}

HRESULT CRoom::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (m_iIndex == -1)
    {
        assert(0);
        return E_FAIL;
    }

    UpdateCenterPosition();

    return S_OK;
}

HRESULT CRoom::PostInitialize()
{
    if (FAILED(CRoomLoadingMgr::GetInstance()->LoadRoomData(m_iIndex)))
    {
        MSG_BOX("[CRoom] Room PostInitialize Failed");
        return E_FAIL;
    }

	return S_OK;
}

void CRoom::UpdateCenterPosition()
{
    _vec3 vTerrainOffset{ 60.f, 0.f, 60.f };

    const int iGridSize = 5;
    _vec2 vMapSize{ 15.f, 13.f };

    int iRoomRow = m_iIndex / iGridSize;
    int iRoomCol = m_iIndex % iGridSize;

    _vec3 vRoomOffset{
        -(float)(iGridSize - 1) / 2.f * vMapSize.x + vMapSize.x * (float)iRoomRow,
        0.f,
        (float)(iGridSize - 1) / 2.f * vMapSize.y - vMapSize.y * (float)iRoomCol,
    };

    m_vCenterPos = _vec3{ vTerrainOffset.x + vRoomOffset.x, 0.f, vTerrainOffset.z + vRoomOffset.z };
    m_pTransformCom->Set_Pos(m_vCenterPos);
}

_int CRoom::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    return iExit;
}

void CRoom::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CRoom::Render_GameObject()
{
}

HRESULT CRoom::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    return S_OK;
}

CRoom* CRoom::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex)
{
    CRoom* pRoom = new CRoom(pGraphicDev, iIndex);

    if (FAILED(pRoom->Ready_GameObject()))
    {
        Safe_Release(pRoom);
        MSG_BOX("CRoom Create Failed");
        return nullptr;
    }

    return pRoom;
}

void CRoom::Free()
{
    CGameObject::Free();
}
