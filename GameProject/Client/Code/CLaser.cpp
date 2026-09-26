#include "pch.h"
#include "CLaser.h"
#include "CProtoMgr.h"
#include "CLaserBuffer.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "Client_Enum.h"
#include "CManagement.h"
#include "CCameraMgr.h"
#include "CImGuiTool.h"
#include "CGameStatusMgr.h"
#include "CRoomLayer.h"
#include "IReflectable.h"

CLaser::CLaser(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir)
    : CProjectile(pGraphicDev), m_vStart(vStart), m_vDir(vDir)
{
}

CLaser::CLaser(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir, const float fTimeAfterBirth, CGameObject* pIgnoreCollision)
    : CProjectile(pGraphicDev), m_vStart(vStart), m_vDir(vDir), m_pPrevGenerationCollidedObject(pIgnoreCollision), m_fBirthTime(fTimeAfterBirth)
{
    m_fTimeAfterBirth = fTimeAfterBirth;
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
    m_pColliderComReflection->Set_Owner(this);
    m_pColliderComReflection->Set_Radius(0.01f);

    return S_OK;
}

_int CLaser::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CProjectile::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);
    // CCollisionMgr::GetInstance()->Add_Collider(COLL_LASER, m_pColliderComReflection);
    CCollisionMgr::GetInstance()->Add_Collider(COLL_PROJECTILE, m_pColliderCom);

    CalculateLength(fTimeDelta);

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
}

void CLaser::OnCollisionEnter(CGameObject* pObject)
{
    if (m_pPrevGenerationCollidedObject == pObject) return;
    if (IReflectable* pReflectable = dynamic_cast<IReflectable*>(pObject))
    {
        /* 충돌체 크기만큼 앞으로 좀 보내기 */
        m_pTransformCom->Move_Pos(&m_vDir, 0.3f, 1.f);

        /* 자식 레이저가 생성되자마자 충돌되는 것 방지 */
        m_pPrevGenerationCollidedObject = pObject;

        /* 자식 레이저 생성 */
        Reflect(pReflectable->GetNormal());

        /* 시각적 어색함을 없애기 위한 길이 상한 */
        m_fReflectLength = m_fCurrentLength;
        m_fReflectTime = m_fTimeAfterBirth;

        /* 더 이상 충돌 처리를 하지 않음 */
        m_pColliderCom->Set_IsActive(false);

        /* 길이 계산식이 변경됨 */
        m_bReflected = true;
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
    CGameStatusMgr::GetInstance()->GetCurrentRoomLayer()->Add_GameObject(L"Projectile_" + to_wstring(pProjectile->GetProjectileID()), pProjectile);

}

HRESULT CLaser::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Mesh
    pComponent = m_pBufferCom = dynamic_cast<CLaserBuffer*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Laser_Buffer"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Laser_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CSphereCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SphereCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });
    
    // Collider
    pComponent = m_pColliderComReflection = dynamic_cast<CSphereCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SphereCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_ReflectCollider", pComponent });

    return S_OK;
}

void CLaser::BillBoardRoll()
{
    CCameraObj* pCamera = CCameraMgr::GetInstance()->GetCamera(L"Camera_Player_FPV");
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