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

CLaser::CLaser(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir)
    : CProjectile(pGraphicDev), m_vStart(vStart), m_vDir(vDir)
{
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


    /* Expanding */
    if (s_tData.fSpeed * m_fTimeAfterBirth < s_tData.fMaxLength)
    {
        m_fCurrentLength = s_tData.fSpeed * m_fTimeAfterBirth;
    }
    /* Shrinking */
    else if (s_tData.fSpeed * (s_tData.fLifeTime - m_fTimeAfterBirth) < s_tData.fMaxLength)
    {
        m_fCurrentLength = (s_tData.fLifeTime - m_fTimeAfterBirth) * s_tData.fSpeed;
    }
    else
    {
        m_fCurrentLength = s_tData.fMaxLength;
    }

    m_pTransformCom->Move_Pos(&m_vDir, s_tData.fSpeed, fTimeDelta);

    m_pTransformCom->Set_Scale(s_tData.fWidth, 1.f, m_fCurrentLength);

    return iExit;
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
    Set_Dead(true);
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

    // Transform
    pComponent = m_pColliderCom = dynamic_cast<CSphereCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SphereCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

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

void CLaser::Free()
{
    CProjectile::Free();
}