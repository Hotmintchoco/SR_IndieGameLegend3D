#include "CGameObject.h"
#include "CComponent.h"
#include "CLayerContext.h"
#include "CTransform.h"
#include "CSphereCollider.h"
#include "CBoxCollider.h"

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev), m_fViewZ(0.f), m_bDead(false),
    m_pOwner(CLayerContext::GetLayer())
{
    m_pGraphicDev->AddRef();
}

CGameObject::CGameObject(const CGameObject& rhs)
    : m_pGraphicDev(rhs.m_pGraphicDev), m_fViewZ(rhs.m_fViewZ), m_bDead(rhs.m_bDead),
    m_pOwner(rhs.m_pOwner)
{
    m_pGraphicDev->AddRef();
}

CGameObject::~CGameObject()
{
}

CComponent* CGameObject::Get_Component(COMPONENTID eID, const _tchar* pComponentTag)
{
    CComponent* pComponent = Find_Component(eID, pComponentTag);

    if (nullptr == pComponent)
        return nullptr;

    return pComponent;
}

HRESULT CGameObject::Ready_GameObject()
{
    for (_uint i = 0; i < ID_END; ++i)
    {
        for (auto& pComponent : m_mapComponent[i])
        {
            pComponent.second->Set_Owner(this);
        }
	}

    return S_OK;
}

_int CGameObject::Update_GameObject(const _float& fTimeDelta)
{
    for (auto& pComponent : m_mapComponent[ID_DYNAMIC])
        pComponent.second->Update_Component(fTimeDelta);

    return 0;
}

void CGameObject::LateUpdate_GameObject(const _float& fTimeDelta)
{
    for (auto& pComponent : m_mapComponent[ID_DYNAMIC])
        pComponent.second->LateUpdate_Component();
}

void CGameObject::Compute_ViewZ(const _vec3* pPos)
{
    _matrix     matCamWorld;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matCamWorld);
    D3DXMatrixInverse(&matCamWorld, 0, &matCamWorld);

    _vec3   vCamPos;
    memcpy(&vCamPos, &matCamWorld.m[3][0], sizeof(_vec3));

    _vec3   vDir = vCamPos - *pPos;
    
    m_fViewZ = D3DXVec3Length(&vDir);
}

void CGameObject::Obstacle_Collision(CGameObject* pOther, CCollider* pObstacleCollider)
{
    if (nullptr == pOther || nullptr == pObstacleCollider)
        return;

    CTransform* pOtherTransformCom = dynamic_cast<CTransform*>(pOther->Get_Component(ID_DYNAMIC, L"Com_Transform"));
    if (nullptr == pOtherTransformCom)
        return;

    CCollider* pOtherCollider = dynamic_cast<CCollider*>(pOther->Get_Component(ID_DYNAMIC, L"Com_Collider"));
    if (nullptr == pOtherCollider)
        pOtherCollider = dynamic_cast<CCollider*>(pOther->Get_Component(ID_STATIC, L"Com_Collider"));

    if (nullptr == pOtherCollider)
        return;

    CSphereCollider* pOtherSphere = dynamic_cast<CSphereCollider*>(pOtherCollider);
    if (nullptr == pOtherSphere)
        return;

    CBoxCollider* pWallBox = dynamic_cast<CBoxCollider*>(pObstacleCollider);
    if (nullptr == pWallBox)
        return;

    if (!pWallBox->Intersect(pOtherSphere))
        return;

    auto Clamp = [](_float v, _float lo, _float hi)->_float
    {
        return (v < lo) ? lo : ((v > hi) ? hi : v);
    };

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

    vPush.y = 0.f;

    _vec3 vPos;
    pOtherTransformCom->Get_Info(INFO_POS, &vPos);
    vPos += vPush;
    pOtherTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

    pOtherCollider->Update_Component(0.f);
}


CComponent* CGameObject::Find_Component(COMPONENTID eID, const _tchar* pComponentTag)
{
    auto        iter = find_if(m_mapComponent[eID].begin(),
                                m_mapComponent[eID].end(), 
                                 CTag_Finder(pComponentTag));

    if (iter == m_mapComponent[eID].end())
        return nullptr;

    return iter->second;
}

void CGameObject::Free()
{
    for (_uint i = 0; i < ID_END; ++i)
    {
        for_each(m_mapComponent[i].begin(), m_mapComponent[i].end(), CDeleteMap());
        m_mapComponent[i].clear();
    }

    Safe_Release(m_pGraphicDev);
}
