#include "pch.h"
#include "CLaser.h"
#include "CProtoMgr.h"
#include "CLaserBuffer.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "Client_Enum.h"
#include "CManagement.h"
#include "CClientCameraMgr.h"
#include "CCamera.h"
#include "CImGuiTool.h"
#include "CRoomLayer.h"
#include "IReflectable.h"
#include "IRayTestable.h"
#include "CRayCaster.h"

CLaser::CLaser(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir)
    : CProjectile(pGraphicDev)
{
    m_vStart = vStart;
    m_vDir = vDir;
}

CLaser::CLaser(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir, const float fTimeAfterBirth, CGameObject* pIgnoreCollision)
    : CProjectile(pGraphicDev), m_pPrevGenerationCollidedObject(pIgnoreCollision), m_fBirthTime(fTimeAfterBirth)
{
    m_fTimeAfterBirth = fTimeAfterBirth;

    m_vStart = vStart;
    m_vDir = vDir;
}

CLaser::~CLaser()
{
}

HRESULT CLaser::Ready_GameObject()
{
    if (FAILED(CProjectile::Ready_GameObject()))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pData = &s_tData;

    m_pTransformCom->Set_Pos(m_vStart);
    m_pColliderCom->Set_Owner(this);
    m_pColliderCom->Set_Radius(0.3f);

    return S_OK;
}

_int CLaser::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CProjectile::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);
    CCollisionMgr::GetInstance()->Add_Collider(COLL_PROJECTILE, m_pColliderCom);

    CalculateLength(fTimeDelta);

    if (!m_bReflected && m_vecRayTestTarget.size() > 0)
    {
        for (auto p : m_vecRayTestTarget)
        {
            PreciseHitTest(p, fTimeDelta);
            if (m_bReflected) break;
        }
    }

    return iExit;
}

void CLaser::CalculateLength(const _float& fTimeDelta)
{
    if (!m_bReflected)
    {
        float fSinceBirth = m_fTimeAfterBirth - m_fBirthTime;

        m_fCurrentLength = min(s_tData.fSpeed * fSinceBirth, s_tData.fMaxLength);

        m_pTransformCom->Move_Pos(&m_vDir, s_tData.fSpeed, fTimeDelta);

        m_pTransformCom->Set_Scale(s_tData.fWidth, 1.f, m_fCurrentLength);
    }
    else
    {
        float fSinceReflect = m_fTimeAfterBirth - m_fReflectTime;

        m_fCurrentLength = min(m_fReflectLength, s_tData.fMaxLength - s_tData.fSpeed * fSinceReflect);

        if (m_fCurrentLength <= 0.f) Set_Dead(true);

        m_pTransformCom->Set_Scale(s_tData.fWidth, 1.f, m_fCurrentLength);
    }
}

void CLaser::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CProjectile::LateUpdate_GameObject(fTimeDelta);

    BillBoardRoll();
}

void CLaser::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(0);

    m_pBufferCom->Render_Buffer();

    if (m_bReflected && s_tData.bShowCorner)
    {
        m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCorner->Get_World());
        m_pTextureComCorner->Set_Texture(0);
        m_pBufferComCorner->Render_Buffer();
    }
}

void CLaser::OnCollisionEnter(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;
    auto pObject = eCollInfo.pOtherCollider->Get_Owner();

    switch (iOtherID)
    {
    case COLLISIONID::COLL_OBSTACLE:
    {
        if (dynamic_cast<IRayTestable*>(pObject))
        {
            // if (m_pPrevGenerationCollidedObject == pObject) break;

            m_vecRayTestTarget.push_back(pObject);
        }
        break;
    }
    case COLLISIONID::COLL_MONSTER:
        /* 몬스터 피격은 몬스터에 이미 구현 */
        break;
    default:
        break;
    }
}

void CLaser::OnCollisionExit(COLLINFO eCollInfo)
{
}

void CLaser::PreciseHitTest(CGameObject* pTarget, const float fTimeDelta)
{
    CRayCaster* pRayCaster = static_cast<CRayCaster*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"RayCaster"));
    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);

    THitInfo t{};

    if (IRayTestable* pRayTestable = dynamic_cast<IRayTestable*>(pTarget))
    {
        vector<pair<CVIBuffer*, CTransform*>> vecInfo = pRayTestable->GetRayTestTargetInfo();
        for (auto& [pBuffer, pTransform] : vecInfo)
        {
            pRayCaster->RayTest(t, vPos, m_vDir, pBuffer, pTransform->Get_World());
        }
    }

    const float fThreshold = s_tData.fSpeed * fTimeDelta;

    if (t.bHit && t.fDist < fThreshold)
    {
        m_pPrevGenerationCollidedObject = pTarget;

        /* 자식 레이저 생성 */
        Reflect(t.fTriNormal);

        /* 시각적 어색함을 없애기 위한 길이 상한 */
        m_fReflectLength = m_fCurrentLength;
        m_fReflectTime = m_fTimeAfterBirth;

        /* 길이 계산식이 변경됨 */
        m_bReflected = true;

        /* 더 이상 충돌 처리를 하지 않음 */
        m_pColliderCom->Set_IsActive(false);
    }
}

void CLaser::Reflect(const _vec3& vNormal)
{
    _vec3 vN;
    D3DXVec3Normalize(&vN, &vNormal);

    _vec3 vReflect = m_vDir - 2.f * D3DXVec3Dot(&m_vDir, &vN) * vN;
    D3DXVec3Normalize(&vReflect, &vReflect);

    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);

    CProjectile* pProjectile = CLaser::Create(m_pGraphicDev, vPos, vReflect, m_fTimeAfterBirth, m_pPrevGenerationCollidedObject);
    CScene* pScene = CManagement::GetInstance()->GetCurrentScene();
    pScene->Add_GameObject(L"Projectile_" + to_wstring(pProjectile->GetProjectileID()), pProjectile);


    if (s_tData.bShowCorner)
    {
        /* 코너용 버퍼 위치 갱신 */
        _vec3 vUp, vLook, vRight;
        vUp = vNormal;
        if (vUp == _vec3{ 1.f, 0.f, 0.f } || vUp == _vec3{ -1.f, 0.f, 0.f })
        {
            vRight = _vec3{ 0.f, -1.f, 0.f }; /* 계산용 가짜 값 */
        }
        else
        {
            vRight = _vec3{ 1.f, 0.f, 0.f }; /* 계산용 가짜 값 */
        }
        D3DXVec3Cross(&vLook, &vRight, &vUp);

        _matrix* pWorld = m_pTransformCorner->Get_World();

        _vec3 vR = s_tData.fWidth * vRight;
        _vec3 vU = s_tData.fWidth * vUp;
        _vec3 vL = s_tData.fWidth * vLook;
        memcpy(&pWorld->m[0][0], &vR, sizeof(_vec3));
        memcpy(&pWorld->m[1][0], &vU, sizeof(_vec3));
        memcpy(&pWorld->m[2][0], &vL, sizeof(_vec3));
        memcpy(&pWorld->m[3][0], &vPos, sizeof(_vec3));
        m_pTransformCorner->WorldMatrixDecompose();
        m_pTransformCorner->Move_Pos(&vUp, 0.04f, 1.f);
    }
}

HRESULT CLaser::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Mesh
    pComponent = m_pBufferCom = dynamic_cast<CLaserBuffer*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Laser_Buffer"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Mesh
    pComponent = m_pBufferComCorner = dynamic_cast<CPlaneTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PlaneTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_BufferCorner", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Laser_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // Texture
    pComponent = m_pTextureComCorner = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Laser_Corner_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_TextureCorner", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CSphereCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SphereCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    // Transform
    pComponent = m_pTransformCorner = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_TransformCorner", pComponent });

    return S_OK;
}

void CLaser::BillBoardRoll()
{
    CCamera* pCamera = CClientCameraMgr::GetInstance()->Get_ActiveCamera();
    if (!pCamera) return;
    _matrix matCamWorld;
    pCamera->GetWorld(&matCamWorld);

    _vec3 vCamPos;
    memcpy(&vCamPos, &matCamWorld.m[3][0], sizeof(_vec3));

    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);

    _vec3 vUp;
    if (!ComputeFacingUp(vCamPos, vPos, m_vDir, &vUp))
    {
        return;
    }

    _vec3 vLook, vRight;
    D3DXVec3Normalize(&vLook, &m_vDir);
    D3DXVec3Cross(&vRight, &vUp, &vLook);

    _matrix* pWorld = m_pTransformCom->Get_World();
    _vec3 vScale = m_pTransformCom->Get_Scale();

    _vec3 vR = vRight * vScale.x;
    _vec3 vU = vUp * vScale.y;
    _vec3 vL = vLook * vScale.z;
    memcpy(&pWorld->m[0][0], &vR, sizeof(_vec3));
    memcpy(&pWorld->m[1][0], &vU, sizeof(_vec3));
    memcpy(&pWorld->m[2][0], &vL, sizeof(_vec3));
}

bool CLaser::ComputeFacingUp(const _vec3& vCamPos, const _vec3& vPos, const _vec3& vDir, _vec3* pOutUp)
{
    _vec3 vLook;
    D3DXVec3Normalize(&vLook, &vDir);

    _vec3 vToCam = vCamPos - vPos;
    _vec3 vN = vToCam - vLook * D3DXVec3Dot(&vToCam, &vLook);   // = Q - H

    float fLen = D3DXVec3Length(&vN);
    if (fLen < 1e-5f)
        return false;

    *pOutUp = vN / fLen;
    return true;
}

void CLaser::RenderEditorPanel()
{
    if (ImGui::Begin("Laser"))
    {
        ImGui::DragFloat("LifeTime", &s_tData.fLifeTime, 0.05f, 0.f, 10.f);
        ImGui::DragFloat("Speed", &s_tData.fSpeed, 0.05f, 0.f, 30.f);
        ImGui::DragFloat("Length", &s_tData.fMaxLength, 0.01f, 0.f, 10.f);
        ImGui::DragFloat("Width", &s_tData.fWidth, 0.01f, 0.f, 5.f);
        ImGui::SliderInt("Reflect Clone", &s_tData.iRefelctionClone, 0, 10);
        ImGui::Checkbox("Corner Complement", &s_tData.bShowCorner);
    }

    ImGui::End();
}

CLaser* CLaser::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir)
{
    CLaser* pBullet = new CLaser(pGraphicDev, vStart, vDir);

    if (FAILED(pBullet->Ready_GameObject()))
    {
        Safe_Release(pBullet);
        MSG_BOX("CLaser Create Failed");
        return nullptr;
    }

    return pBullet;
}

CLaser* CLaser::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir, const float fTimeAfterBirth, CGameObject* pIgnoreCollision)
{
    CLaser* pBullet = new CLaser(pGraphicDev, vStart, vDir, fTimeAfterBirth, pIgnoreCollision);

    if (FAILED(pBullet->Ready_GameObject()))
    {
        Safe_Release(pBullet);
        MSG_BOX("CLaser Create Failed");
        return nullptr;
    }

    return pBullet;
}

void CLaser::Free()
{
    CProjectile::Free();
}
