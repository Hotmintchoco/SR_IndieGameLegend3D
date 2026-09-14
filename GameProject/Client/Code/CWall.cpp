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
        pLayer->m_OnRoomBegin.AddBinding(GetToken(), [this]() { OnRoomBegin(); });
    }

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

    // -- Collider Initialization -- 
}

void CWall::OnRoomBegin()
{
    cout << "이벤트 처리됨" << endl;
    int a = 0;
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
        CCollisionMgr::GetInstance()->Add_Collider(COLL_WALL, m_pColliderCom[i]);
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
    if (nullptr == pOther)
        return;

    CTransform* pOtherTransformCom = dynamic_cast<CTransform*>(pOther->Get_Component(ID_DYNAMIC, L"Com_Transform"));
    if (nullptr == pOtherTransformCom)
        return;

    CCollider* pOtherCollider = dynamic_cast<CCollider*>(pOther->Get_Component(ID_DYNAMIC, L"Com_Collider"));
    if (nullptr == pOtherCollider)
        pOtherCollider = dynamic_cast<CCollider*>(pOther->Get_Component(ID_STATIC, L"Com_Collider"));

    if (nullptr == pOtherCollider)
        return;

    // 현재는 구형(CSphereCollider) 기준 처리
    CSphereCollider* pOtherSphere = dynamic_cast<CSphereCollider*>(pOtherCollider);
    if (nullptr == pOtherSphere)
        return;

    auto Clamp = [](_float v, _float lo, _float hi)->_float
    {
        return (v < lo) ? lo : ((v > hi) ? hi : v);
    };

    for (int i = 0; i < 2; ++i)
    {
        CBoxCollider* pWallBox = dynamic_cast<CBoxCollider*>(m_pColliderCom[i]);
        if (nullptr == pWallBox)
            continue;

        if (!pWallBox->Intersect(pOtherSphere))
            continue;

        const _vec3 vCenter = {
            pOtherSphere->m_tSphere.Center.x,
            pOtherSphere->m_tSphere.Center.y,
            pOtherSphere->m_tSphere.Center.z
        };

        const _vec3 vBoxCenter = {
            pWallBox->m_tBox.Center.x,
            pWallBox->m_tBox.Center.y,
            pWallBox->m_tBox.Center.z
        };

        const _vec3 vExt = {
            pWallBox->m_tBox.Extents.x,
            pWallBox->m_tBox.Extents.y,
            pWallBox->m_tBox.Extents.z
        };

        const _vec3 vMin = { vBoxCenter.x - vExt.x, vBoxCenter.y - vExt.y, vBoxCenter.z - vExt.z };
        const _vec3 vMax = { vBoxCenter.x + vExt.x, vBoxCenter.y + vExt.y, vBoxCenter.z + vExt.z };

        _vec3 vClosest = {
            Clamp(vCenter.x, vMin.x, vMax.x),
            Clamp(vCenter.y, vMin.y, vMax.y),
            Clamp(vCenter.z, vMin.z, vMax.z)
        };

        _vec3 vDelta = vCenter - vClosest;
        _float fDist = D3DXVec3Length(&vDelta);
        const _float fRadius = pOtherSphere->m_tSphere.Radius;
        const _float fSkin = 0.001f;

        _vec3 vPush(0.f, 0.f, 0.f);

        if (fDist > FLT_EPSILON)
        {
            _vec3 vN;
            D3DXVec3Normalize(&vN, &vDelta);
            const _float fPen = (fRadius - fDist) + fSkin;
            if (fPen > 0.f)
                vPush = vN * fPen;
        }
        else
        {
            // 구 중심이 박스 내부에 가까운 특수 케이스
            const _float dxMin = vCenter.x - vMin.x;
            const _float dxMax = vMax.x - vCenter.x;
            const _float dzMin = vCenter.z - vMin.z;
            const _float dzMax = vMax.z - vCenter.z;

            _float fMinPen = dxMin;
            vPush = { -(fRadius + fSkin), 0.f, 0.f };

            if (dxMax < fMinPen) { fMinPen = dxMax; vPush = { +(fRadius + fSkin), 0.f, 0.f }; }
            if (dzMin < fMinPen) { fMinPen = dzMin; vPush = { 0.f, 0.f, -(fRadius + fSkin) }; }
            if (dzMax < fMinPen) { vPush = { 0.f, 0.f, +(fRadius + fSkin) }; }
        }

        // 벽 충돌은 수평 보정만 적용 (점프/중력과 충돌 방지)
        vPush.y = 0.f;

        _vec3 vPos;
        pOtherTransformCom->Get_Info(INFO_POS, &vPos);
        vPos += vPush;
        pOtherTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

        // 다음 벽 콜라이더 검사 전에 상대 콜라이더 중심 갱신
        pOtherCollider->Update_Component(0.f);
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
        L"Com_Collider_0",
        L"Com_Collider_1",
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
