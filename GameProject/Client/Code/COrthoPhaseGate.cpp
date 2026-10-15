#include "pch.h"
#include "COrthoPhaseGate.h"
#include "CProtoMgr.h"
#include "CBoxCollider.h"
#include "CCollisionMgr.h"
#include "CRenderer.h"
#include "COrthoRoomLayer.h"
#include "COrthoCamera.h"
#include "CRcTex.h"
#include "CTexture.h"

COrthoPhaseGate::COrthoPhaseGate(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

COrthoPhaseGate::~COrthoPhaseGate()
{
}

HRESULT COrthoPhaseGate::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    /* 한 칸 크기의 사각형, 기본은 바닥에 눕힌 상태 */
    m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
    m_pTransformCom->Set_Rotation_Raw(_vec3{ 90.f, 0.f, 0.f });

    /* 스프라이트 프레임 수 */
    m_iTotalFrameCount = m_pTextureCom->GetCount();

    /* 바닥의 플랫 플레이어와 바닥 위의 플레이어가 둘 다 닿을 수 있도록 위아래로 넉넉하게 */
    m_pColliderCom->Set_Extents(_vec3{ 0.5f, 1.0f, 0.5f });
    m_pColliderCom->Set_DiffPos(_vec3{ 0.0f, 0.5f, 0.0f });

    return S_OK;
}

_int COrthoPhaseGate::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    /* 스프라이트 애니메이션 : 누적 시간으로 현재 프레임 계산 (한 바퀴 돌면 시간도 되감음) */
    if (m_iTotalFrameCount > 0)
    {
        m_fTimeAfterSpawn += fTimeDelta;

        const float fLoopTime = m_fFrameInterval * (float)m_iTotalFrameCount;
        while (m_fTimeAfterSpawn >= fLoopTime)
            m_fTimeAfterSpawn -= fLoopTime;

        m_iCurrentTextureIndex = (int)(m_fTimeAfterSpawn / m_fFrameInterval);
        if (m_iCurrentTextureIndex >= m_iTotalFrameCount)
            m_iCurrentTextureIndex = m_iTotalFrameCount - 1;
    }

    /* 투명 배경 스프라이트라 알파 테스트 그룹 */
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);
    CCollisionMgr::GetInstance()->Add_Collider(COLL_ROOMLOGIC, m_pColliderCom);

    return iExit;
}

void COrthoPhaseGate::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    COrthoRoomLayer* pRoom = dynamic_cast<COrthoRoomLayer*>(m_pOwner);
    if (nullptr == pRoom) return;

    /* 시점이 바뀌는 순간 높이 전환 : 직교면 2D 플레이어 아래로, 원근이면 원래 높이로 */
    const bool bOrtho = (pRoom->GetGameMode() == ORTHO);
    if (bOrtho != m_bLoweredForFlat)
    {
        _vec3 vPos;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);

        if (bOrtho)
        {
            m_fOriginY = vPos.y;
            vPos.y = s_fFlatModeY;
        }
        else
        {
            vPos.y = m_fOriginY;
        }

        m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
        m_bLoweredForFlat = bOrtho;
    }

    if (pRoom->GetGameMode() == PERSP)
    {
        /* 원근 : 직교 카메라 기준 빌보드 */
        if (COrthoCamera* pCamera = pRoom->GetOrthoCamera())
            BillBoard(pCamera);
    }
    else
    {
        /* 직교 : 회전을 처음(바닥에 눕힌 상태)으로 되돌림 */
        m_pTransformCom->Set_Rotation_Raw(_vec3{ 90.f, 0.f, 0.f });
    }
}

void COrthoPhaseGate::BillBoard(CCamera* pCamera)
{
    _matrix matCamWorld;
    pCamera->GetWorld(&matCamWorld);

    /* 카메라 - 게이트 변위 벡터를 vLook으로 하는 world matrix를 만듬 */
    _vec3 vCamPos, vGatePos, vDisplacement, vLook;
    memcpy(&vCamPos, &matCamWorld.m[3][0], sizeof(_vec3));
    m_pTransformCom->Get_Info(INFO_POS, &vGatePos);
    vDisplacement = vGatePos - vCamPos;
    D3DXVec3Normalize(&vLook, &vDisplacement);

    /* up (0, 1, 0) : right 구하기 위한 방향성만 가짐 */
    _vec3 vUp{ 0.f, 1.f, 0.f };
    _vec3 vRight;
    D3DXVec3Cross(&vRight, &vUp, &vDisplacement);
    D3DXVec3Normalize(&vRight, &vRight);
    D3DXVec3Cross(&vUp, &vLook, &vRight);

    /* 스케일이 무시되면 안되므로 직접 적용 */
    _vec3 vScale = m_pTransformCom->Get_Scale();
    _vec3 vRightS = vRight * vScale.x;
    _vec3 vUpS = vUp * vScale.y;
    _vec3 vLookS = vLook * vScale.z;

    _matrix* pMatWorld = m_pTransformCom->Get_World();

    memcpy(&pMatWorld->m[0][0], &vRightS, sizeof(_vec3));
    memcpy(&pMatWorld->m[1][0], &vUpS, sizeof(_vec3));
    memcpy(&pMatWorld->m[2][0], &vLookS, sizeof(_vec3));
}

void COrthoPhaseGate::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture(m_iCurrentTextureIndex);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pBufferCom->Render_Buffer();
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void COrthoPhaseGate::OnCollisionEnter(COLLINFO eCollInfo)
{
    SetOwnerSwitch(eCollInfo.iOtherID, true);
}

void COrthoPhaseGate::OnCollisionExit(COLLINFO eCollInfo)
{
    SetOwnerSwitch(eCollInfo.iOtherID, false);
}

void COrthoPhaseGate::SetOwnerSwitch(_int iOtherID, bool bFlag)
{
    COrthoRoomLayer* pRoom = dynamic_cast<COrthoRoomLayer*>(m_pOwner);
    if (nullptr == pRoom) return;

    switch (iOtherID)
    {
    case COLLISIONID::COLL_PLAYER:
        pRoom->SetPlayerOnGate(bFlag);
        break;
    case COLLISIONID::COLL_FLATPLAYER:
        pRoom->SetFlatPlayerOnGate(bFlag);
        break;
    default:
        break;
    }
}

HRESULT COrthoPhaseGate::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CBoxCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BoxCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    // Buffer
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Portal_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

COrthoPhaseGate* COrthoPhaseGate::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    COrthoPhaseGate* pObject = new COrthoPhaseGate(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("COrthoPhaseGate Create Failed");
        return nullptr;
    }

    return pObject;
}

void COrthoPhaseGate::Free()
{
    CGameObject::Free();
}
