#include "pch.h"
#include "CLiminalPortal.h"
#include "Client_Enum.h"
#include "CProtoMgr.h"
#include "CPlayer.h"
#include "CDefaultBullet.h"
#include "CPlayerMovement.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "CClientCameraMgr.h"
#include "CCamera.h"
#include "CStage.h"
#include "CManagement.h"

namespace
{
    /* Liminal_Portal_*.ply 기준 (블럭 1 = 1.0, 원점 = 바닥 중앙)
       Extents는 반크기. Blender X → X, Blender Z(위) → Y, Blender Y(두께) → Z */

       // 기둥: x ±1~2, 높이 0~4, 두께 1
    const _vec3 FRAME_PILLAR_EXTENTS = { 0.5f, 2.f, 0.5f };
    const _vec3 FRAME_PILLAR_L_OFFSET = { -1.5f, 2.f, 0.f };
    const _vec3 FRAME_PILLAR_R_OFFSET = { 1.5f, 2.f, 0.f };

    const _vec3 ANIM_EXTENTS = { 1.f, 1.5f, 1.f };
    const _vec3 ANIM_OFFSET = { 0.f, 1.5f, 0.f };
}

CLiminalPortal::CLiminalPortal(LPDIRECT3DDEVICE9 pGraphicDev)
    : CLiminalObject(pGraphicDev)
{
}

CLiminalPortal::~CLiminalPortal()
{
}

HRESULT CLiminalPortal::Ready_GameObject()
{
    /* 포탈 전용 컴포넌트 먼저 등록 → 부모 Ready에서 공통 컴포넌트 + CGameObject::Ready 처리 */
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CLiminalObject::Ready_GameObject()))
        return E_FAIL;

    m_iAnimFrameCount = m_pAnimTexture->GetCount();

    return S_OK;
}

_int CLiminalPortal::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CLiminalObject::Update_GameObject(fTimeDelta);
    
    if (m_bTeleportGuard)
    {
        m_fLeftGuartTime -= fTimeDelta;
        if (m_fLeftGuartTime <= 0.f)
        {
            m_bTeleportGuard = false;
        }
    }

    m_fScale = m_pTransformCom->Get_Scale().x;

    // CCollisionMgr::GetInstance()->Add_Collider(COLL_OBSTACLE, m_pFrameColliderL);
    // CCollisionMgr::GetInstance()->Add_Collider(COLL_OBSTACLE, m_pFrameColliderR);
    CCollisionMgr::GetInstance()->Add_Collider(COLL_ROOMLOGIC, m_pAnimCollider);

    if (m_iAnimFrameCount > 0)
    {
        m_fAnimAccTime += fTimeDelta;
        while (m_fAnimAccTime >= m_fAnimFrameInterval)
        {
            m_fAnimAccTime -= m_fAnimFrameInterval;
            m_iCurrentAnimFrame = (m_iCurrentAnimFrame + 1) % m_iAnimFrameCount;
        }
    }

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CLiminalPortal::LateUpdate_GameObject(_float fTimeDelta)
{
    CLiminalObject::LateUpdate_GameObject(fTimeDelta);

    CheckCrossCondition();
}

void CLiminalPortal::Render_GameObject()
{
    if (m_bGrabbed)
    {
        m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pOutlineTransformCom->Get_World());
        m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

        m_pOutlineTextureCom->Set_Texture(100);
        m_pBufferCom->Render_Buffer();

        m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    }

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    /* 프레임 */
    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();

    /* 포탈 애니메이션 */
    m_pAnimTexture->Set_Texture(m_iCurrentAnimFrame);
    m_pAnimBuffer->Render_Buffer();
}

HRESULT CLiminalPortal::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Frame Buffer
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_LiminalPortal_Frame_Buffer"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Frame Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_LiminalPortal_Frame_Texture"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // Anim Buffer
    pComponent = m_pAnimBuffer = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_LiminalPortal_Anim_Buffer"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_AnimBuffer", pComponent });

    // Anim Texture
    pComponent = m_pAnimTexture = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_LiminalPortal_Anim_Texture"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_AnimTexture", pComponent });

    // // Frame Colliders (양 기둥, 장애물) - 키가 겹치면 insert가 무시되므로 반드시 다른 키
    // m_pFrameColliderL = Add_BoxCollider(L"Com_FrameColliderL", FRAME_PILLAR_EXTENTS, FRAME_PILLAR_L_OFFSET);
    // m_pFrameColliderR = Add_BoxCollider(L"Com_FrameColliderR", FRAME_PILLAR_EXTENTS, FRAME_PILLAR_R_OFFSET);
    // if (nullptr == m_pFrameColliderL || nullptr == m_pFrameColliderR)
    //     return E_FAIL;
    // // TODO: 기존 장애물 콜라이더와 같은 CollisionID 지정
    // m_pFrameColliderL->Set_IsTrigger(false);
    // m_pFrameColliderR->Set_IsTrigger(false);

    // Anim Collider (포탈면 트리거)
    m_pAnimCollider = Add_BoxCollider(L"Com_AnimCollider", ANIM_EXTENTS, ANIM_OFFSET);
    if (nullptr == m_pAnimCollider)
        return E_FAIL;
    m_pAnimCollider->Set_IsTrigger(true);
    m_pAnimCollider->Set_CollisionID(COLL_ROOMLOGIC);

    return S_OK;
}

CBoxCollider* CLiminalPortal::Add_BoxCollider(const _tchar* pComponentKey, const _vec3& vExtents, const _vec3& vDiffPos)
{
    CBoxCollider* pCollider = dynamic_cast<CBoxCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BoxCollider"));
    if (nullptr == pCollider)
        return nullptr;

    pCollider->Set_Extents(vExtents);
    pCollider->Set_DiffPos(vDiffPos);   // 현재는 오너 위치 + DiffPos (스케일/회전 미반영)

    /* LateUpdate에서 SyncPositionToOwner가 돌아야 하므로 DYNAMIC */
    m_mapComponent[ID_DYNAMIC].insert({ pComponentKey, pCollider });
    return pCollider;
}

vector<pair<Engine::CVIBuffer*, Engine::CTransform*>> CLiminalPortal::GetRayTestTargetInfo()
{
    return {
        { m_pBufferCom,  m_pTransformCom },
        { m_pAnimBuffer, m_pTransformCom }
    };
}

void CLiminalPortal::OnCollisionEnter(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    // auto iter = m_setTeleportGuardList.find(pOtherCol->Get_Owner());
    // if (iter != m_setTeleportGuardList.end()) return;
    // 
    // if (iMyID != COLL_ROOMLOGIC) return;
    // 
    // if (m_bGrabbed) return;
    // 
    // CGameObject* pObject = pOtherCol->Get_Owner();
    // CTransform* pTransform = static_cast<CTransform*>(pObject->Get_Component(ID_DYNAMIC, L"Com_Transform"));
    // switch (iOtherID)
    // {
    // case COLL_PLAYER:
    // {
    //     CTransform* pYawTransform = static_cast<CTransform*>(pObject->Get_Component(ID_DYNAMIC, L"Com_VisualRootTransform"));
    //     _matrix matPlayerWorld, matPlayerYaw, matPlayerBase;
    //     matPlayerBase = *pTransform->Get_World();
    //     matPlayerYaw = *pYawTransform->Get_World();
    //     matPlayerWorld = matPlayerYaw * matPlayerBase;
    //     TeleportTarget(pObject, matPlayerWorld);
    //     break;
    // }
    // case COLL_PROJECTILE:
    // {
    //     if (CDefaultBullet* pTarget = dynamic_cast<CDefaultBullet*>(pObject))
    //     {
    //         TeleportTarget(pTarget, *pTransform->Get_World());
    //     }
    //     break;
    // }
    // default:
    //     break;
    // }
}

void CLiminalPortal::OnCollisionStay(COLLINFO eCollInfo)
{
    /* 일단 비활성화 */
    // auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;
    // 
    // if (iMyID != COLL_OBSTACLE) return;
    // 
    // switch (iOtherID)
    // {
    // case COLLISIONID::COLL_PLAYER:
    //     Obstacle_Collision(eCollInfo.pOtherCollider, m_pColliderCom);
    //     break;
    // default:
    //     break;
    // }
}

void CLiminalPortal::OnCollisionExit(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    // if (iMyID != COLL_ROOMLOGIC) return;
    // 
    // switch (iOtherID)
    // {
    // case COLL_PLAYER:
    // case COLL_PROJECTILE:
    // {
    //     auto iter = m_setTeleportGuardList.find(pOtherCol->Get_Owner());
    //     if (iter != m_setTeleportGuardList.end())
    //     {
    //         m_setTeleportGuardList.erase(iter);
    //     }
    //     break;
    // }
    // default:
    //     break;
    // }
}

void CLiminalPortal::TeleportTarget(CGameObject* pTarget, const _matrix& matTargetWorld)
{
    float fPerspectiveScale = m_pLinkedPortal->GetScale() / m_fScale;
    _matrix matMyWorld = *m_pTransformCom->Get_World();
    _matrix matOtherWorld = *m_pLinkedPortal->GetTransform()->Get_World();
    _matrix matMyWorldInv;
    D3DXMatrixInverse(&matMyWorldInv, nullptr, &matMyWorld);
    _matrix matRotY;
    D3DXMatrixRotationY(&matRotY, D3DX_PI);
    _matrix matTeleport = matMyWorldInv * matRotY * matOtherWorld;

    /* ---- 디버그 헬퍼 ---- */
    auto PrintVec = [](const char* szTag, const _vec3& v)
        {
            cout << szTag << " (" << v.x << ", " << v.y << ", " << v.z << ")"
                << " | len : " << D3DXVec3Length(&v) << endl;
        };
    auto PrintMat = [](const char* szTag, const _matrix& m)
        {
            cout << szTag << endl;
            for (int i = 0; i < 4; ++i)
                cout << "  [" << m.m[i][0] << ", " << m.m[i][1] << ", " << m.m[i][2] << ", " << m.m[i][3] << "]" << endl;
        };

    // cout << "========== Portal Teleport ==========" << endl;
    // cout << "내 포탈 스케일 : " << m_fScale
    //     << ", 연결 포탈 스케일 : " << m_pLinkedPortal->GetScale()
    //     << ", PerspectiveScale : " << fPerspectiveScale << endl;
    // PrintMat("[matMyWorld]", matMyWorld);
    // PrintMat("[matOtherWorld]", matOtherWorld);
    // PrintMat("[matTeleport]", matTeleport);

    /* 속도, 방향은 회전 변환 후 반사 처리가 되어야 한다 */

    if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(pTarget))
    {
        //cout << "--- 대상 : Player ---" << endl;

        /* 스케일 조정 */
        float fPlayerScale = pPlayer->GetPseudoScale();
        float s = clamp(fPlayerScale * fPerspectiveScale, 0.03f, 1.f);
        pPlayer->SetPseudoScale(s);
        //cout << "스케일 : " << fPlayerScale << " -> " << s << endl;

        /* 위치 조정 */
        _vec3 vPlayerPos = pPlayer->GetTransform()->Get_Info_Value(INFO_POS);
        //PrintVec("위치 (이전)", vPlayerPos);
        D3DXVec3TransformCoord(&vPlayerPos, &vPlayerPos, &matTeleport);
        pPlayer->GetTransform()->Set_Pos(vPlayerPos);
        //PrintVec("위치 (이후)", vPlayerPos);

        /* 속도 방향 전환 */
        _vec3 vPlayerVelocity = pPlayer->GetMovement()->GetVelocity();
        //PrintVec("속도 (이전)", vPlayerVelocity);
        D3DXVec3TransformNormal(&vPlayerVelocity, &vPlayerVelocity, &matTeleport);
        pPlayer->GetMovement()->SetVelocity(vPlayerVelocity);
        //PrintVec("속도 (이후)", vPlayerVelocity);

        /* 카메라, 플레이어 방향 */
        CCamera* pCamera = CClientCameraMgr::GetInstance()->Find_Camera(CLIENT_CAMERA_TYPE::PLAYER);
        _matrix matCamWorld;
        pCamera->GetWorld(&matCamWorld);
        _vec3 vCamLook;
        memcpy(&vCamLook, &matCamWorld.m[2][0], sizeof(_vec3));
        D3DXVec3TransformNormal(&vCamLook, &vCamLook, &matTeleport);
        float fYaw = atan2f(vCamLook.x, vCamLook.z);
        pCamera->Rotate(fYaw - pCamera->Get_Yaw(), 0.f);
        pPlayer->SyncCameraYaw();

        m_pLinkedPortal->EnableGuard();
        //cout << "TeleportGuard 설정 완료 (Player)" << endl;
    }
    // else if (CDefaultBullet* pBullet = dynamic_cast<CDefaultBullet*>(pTarget))
    // {
    //     //cout << "--- 대상 : Bullet ---" << endl;
    // 
    //     /* 스케일 조정 */
    //     float fBombScale = pBullet->GetScale();
    //     pBullet->SetScale(fBombScale * fPerspectiveScale);
    //     //cout << "스케일 : " << fBombScale << " -> " << fBombScale * fPerspectiveScale << endl;
    // 
    //     /* 위치 조정 */
    //     CTransform* pTransform = dynamic_cast<CTransform*>(pBullet->Get_Component(ID_DYNAMIC, L"Com_Transform"));
    //     if (!pTransform)
    //     {
    //         //cout << "[경고] Bullet Transform이 nullptr!" << endl;
    //     }
    // 
    //     _vec3 vBulletPos = pTransform->Get_Info_Value(INFO_POS);
    //     //PrintVec("위치 (이전)", vBulletPos);
    //     D3DXVec3TransformCoord(&vBulletPos, &vBulletPos, &matTeleport);
    //     pTransform->Set_Pos(vBulletPos);
    //     //PrintVec("위치 (이후)", vBulletPos);
    // 
    //     /* 속도 방향 전환 */
    //     _vec3 vBulletDir = pBullet->GetDir();
    //     //PrintVec("방향 (이전)", vBulletDir);
    //     D3DXVec3TransformNormal(&vBulletDir, &vBulletDir, &matTeleport);
    //     pBullet->SetDir(vBulletDir);
    //     //PrintVec("방향 (이후)", vBulletDir);
    // 
    //     m_pLinkedPortal->EnableGuard();
    //     //cout << "TeleportGuard 설정 완료 (Bullet)" << endl;
    // }
    // else
    // {
    //     //cout << "--- 대상 : 처리되지 않은 타입 ---" << endl;
    // }
    // //cout << "=====================================" << endl;
}

void CLiminalPortal::CheckCrossCondition()
{
    if (m_bTeleportGuard) return;

    bool bCrossed = false;

    CStage* pStage = static_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
    if (!pStage) return;
    
    _vec3 vPos = pStage->GetPlayerPosition();
    _matrix matPortalWorld = *m_pTransformCom->Get_World();
    _matrix matPortalWorldInv;
    D3DXMatrixInverse(&matPortalWorldInv, nullptr, &matPortalWorld);
    D3DXVec3TransformCoord(&vPos, &vPos, &matPortalWorldInv);
    
    bCrossed = (-1.f <= vPos.y && vPos.y <= 3.f)
        && (-1.f <= vPos.x && vPos.x <= 1.f) 
        && (m_vPrevPos.z > 0.f && vPos.z <= 0.f);
    
    if (bCrossed)
    {
        CTransform* pTransform = pStage->GetPlayer()->GetTransform();
        CTransform* pYawTransform = static_cast<CTransform*>(pStage->GetPlayer()->Get_Component(ID_DYNAMIC, L"Com_VisualRootTransform"));
        _matrix matPlayerWorld, matPlayerYaw, matPlayerBase;
        matPlayerBase = *pTransform->Get_World();
        matPlayerYaw = *pYawTransform->Get_World();
        matPlayerWorld = matPlayerYaw * matPlayerBase;
        TeleportTarget(pStage->GetPlayer(), matPlayerWorld);
    }

    m_vPrevPos = vPos;
}


CLiminalPortal* CLiminalPortal::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CLiminalPortal* pObject = new CLiminalPortal(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CLiminalPortal Create Failed");
        return nullptr;
    }

    return pObject;
}

void CLiminalPortal::Free()
{
    /* 컴포넌트는 m_mapComponent에서 CGameObject::Free가 해제. m_pLinkedPortal은 약한 참조라 Release 안 함 */
    CLiminalObject::Free();
}