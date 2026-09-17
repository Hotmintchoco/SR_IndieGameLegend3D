#include "CBoxCollider.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "CSphereCollider.h"
#include "CProtoMgr.h"
#include "CCubeTex.h"

CBoxCollider::CBoxCollider()
{
    m_eColliderType = CT_BOX;
}

CBoxCollider::CBoxCollider(LPDIRECT3DDEVICE9 pGraphicDev)
    : CCollider(pGraphicDev)
{
    m_eColliderType = CT_BOX;
    m_pDebugCubeTex = dynamic_cast<CCubeTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CubeTex"));
}

CBoxCollider::CBoxCollider(const CBoxCollider& rhs)
    : CCollider(rhs), m_tBox(rhs.m_tBox), m_pDebugCubeTex(rhs.m_pDebugCubeTex)
{
    if (nullptr != m_pDebugCubeTex)
        m_pDebugCubeTex->AddRef();
}

CBoxCollider::~CBoxCollider()
{
}

_bool CBoxCollider::Intersect(CCollider* pOther)
{
    if (nullptr == pOther)
        return false;

    if (pOther->Get_ColliderType() == CT_BOX)
    {
        CBoxCollider* pTargetBox = static_cast<CBoxCollider*>(pOther);
        return m_tBox.Intersects(pTargetBox->m_tBox);
    }

    if (pOther->Get_ColliderType() == CT_SPHERE)
    {
        CSphereCollider* pTargetSphere = static_cast<CSphereCollider*>(pOther);
        return m_tBox.Intersects(pTargetSphere->m_tSphere);
    }

    return false;
}

_int CBoxCollider::Update_Component(const _float& fTimeDelta)
{
    if (nullptr == m_pOwner)
        return 0;

    CTransform* pOwnerTransformCom = dynamic_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, L"Com_Transform"));
    if (nullptr == pOwnerTransformCom)
        return 0;

    _vec3 vOwnerPos;
    pOwnerTransformCom->Get_Info(INFO_POS, &vOwnerPos);
	vOwnerPos += m_vDiffPos;

    m_tBox.Center = { vOwnerPos.x, vOwnerPos.y, vOwnerPos.z };

    return 0;
}

void CBoxCollider::LateUpdate_Component()
{
}

void CBoxCollider::Set_Extents(const _vec3& vExtents)
{
    m_tBox.Extents = { vExtents.x, vExtents.y, vExtents.z };
}

void CBoxCollider::Set_Extents(const _float& fX, const _float& fY, const _float& fZ)
{
    m_tBox.Extents = { fX, fY, fZ };
}

void CBoxCollider::Set_Radius(const _float& fRadius)
{
    m_fRadius = fRadius;
    m_tBox.Extents = { fRadius, fRadius, fRadius };
}

void CBoxCollider::Render_DebugCube()
{
#ifdef _DEBUG
    if (nullptr == m_pDebugCubeTex || nullptr == m_pGraphicDev)
        return;

    _matrix matScale, matTrans, matWorld, matOldWorld;
    D3DXMatrixScaling(&matScale, m_tBox.Extents.x, m_tBox.Extents.y, m_tBox.Extents.z);
    D3DXMatrixTranslation(&matTrans, m_tBox.Center.x, m_tBox.Center.y, m_tBox.Center.z);
    matWorld = matScale * matTrans;

    DWORD dwOldFill = D3DFILL_SOLID;
    DWORD dwOldCull = D3DCULL_CCW;
    DWORD dwOldLighting = TRUE;

    m_pGraphicDev->GetTransform(D3DTS_WORLD, &matOldWorld);
    m_pGraphicDev->GetRenderState(D3DRS_FILLMODE, &dwOldFill);
    m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dwOldCull);
    m_pGraphicDev->GetRenderState(D3DRS_LIGHTING, &dwOldLighting);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pDebugCubeTex->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, dwOldLighting);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, dwOldCull);
    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, dwOldFill);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matOldWorld);
#endif
}

CCollider* CBoxCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    return new CBoxCollider(pGraphicDev);
}

CComponent* CBoxCollider::Clone()
{
    return new CBoxCollider(*this);
}

void CBoxCollider::Free()
{
    Safe_Release(m_pDebugCubeTex);
    CCollider::Free();
}