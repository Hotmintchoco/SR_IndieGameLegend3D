#include "pch.h"
#include "CBulletTrail.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"

namespace
{
    const _float    TRAIL_HALFWIDTH = 0.1f;
    const _float    TRAIL_LENGTH = 3.f;
}



CBulletTrail::CBulletTrail(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_vDir(0.f, 0.f, 0.f), m_fSpeed(0.f)
{
}


CBulletTrail::~CBulletTrail()
{
}

HRESULT CBulletTrail::Ready_GameObject(const _vec3* pPos, const _vec3* pDir, _float fSpeed)
{
    D3DXVec3Normalize(&m_vDir, pDir);

    m_fSpeed = fSpeed;



    vector<VTXTEX>      vecVtx(4);
    vector<INDEX16>     vecIdx(2);

    vecVtx[0].vPosition = { -TRAIL_HALFWIDTH, 0.f, 0.f };
    vecVtx[0].vTexUV = { 0.f, 0.f };

    vecVtx[1].vPosition = { TRAIL_HALFWIDTH, 0.f, 0.f };
    vecVtx[1].vTexUV = { 1.f, 0.f };

    vecVtx[2].vPosition = { TRAIL_HALFWIDTH, 0.f, -TRAIL_LENGTH };
    vecVtx[2].vTexUV = { 1.f, 1.f };

    vecVtx[3].vPosition = { -TRAIL_HALFWIDTH, 0.f, -TRAIL_LENGTH };
    vecVtx[3].vTexUV = { 0.f, 1.f };

    for (auto& rVtx : vecVtx)
        rVtx.vNormal = { 0.f, 1.f, 0.f };

    vecIdx[0] = { 0, 1, 2 };
    vecIdx[1] = { 0, 2, 3 };

    if (FAILED(Add_Component(vecVtx, vecIdx)))
        return E_FAIL;

    m_pTransformCom->Set_Pos(pPos->x, pPos->y, pPos->z);

    __super::Ready_GameObject();

    return S_OK;
}

_int CBulletTrail::Update_GameObject(const _float& fTimeDelta)
{
    m_pTransformCom->Move_Pos(&m_vDir, m_fSpeed, fTimeDelta);
    
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return iExit;
}

void CBulletTrail::LateUpdate_GameObject(const _float& fTimeDelta)
{
    _vec3   vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);

    CGameObject::Compute_ViewZ(&vPos);


    _matrix matView, matCamWorld;

    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matCamWorld, 0, &matView);

    _vec3   vCamPos;
    memcpy(&vCamPos, &matCamWorld.m[INFO_POS][0], sizeof(_vec3));

    _vec3   vToCam = vCamPos - vPos;
    _vec3   vRight;

    D3DXVec3Cross(&vRight, &vToCam, &m_vDir);

    if (D3DXVec3LengthSq(&vRight) < 0.0001f)    
    {
        _vec3   vWorldUp = { 0.f, 1.f, 0.f };
        D3DXVec3Cross(&vRight, &vWorldUp, &m_vDir);
    }

    D3DXVec3Normalize(&vRight, &vRight);

    _vec3   vUp;

    D3DXVec3Cross(&vUp, &m_vDir, &vRight);
    D3DXVec3Normalize(&vUp, &vUp);

    _matrix matWorld = *m_pTransformCom->Get_World();

    memcpy(&matWorld.m[INFO_RIGHT][0], &vRight, sizeof(_vec3));
    memcpy(&matWorld.m[INFO_UP][0], &vUp, sizeof(_vec3));
    memcpy(&matWorld.m[INFO_LOOK][0], &m_vDir, sizeof(_vec3));

    m_pTransformCom->Set_World(&matWorld);

    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CBulletTrail::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture(0);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}


HRESULT CBulletTrail::Add_Component(vector<VTXTEX>& vecVtx, vector<INDEX16>& vecIdx)
{
    CComponent* pComponent = nullptr;

    // Buffer
    pComponent = m_pBufferCom = CCustomTex::Create(m_pGraphicDev, vecVtx, vecIdx);

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", pComponent });

    // Texture

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BulletTrailTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });


    return S_OK;
}


CBulletTrail* CBulletTrail::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pPos, const _vec3* pDir, _float fSpeed)
{
    CBulletTrail* pBulletTrail = new CBulletTrail(pGraphicDev);

    if (FAILED(pBulletTrail->Ready_GameObject(pPos, pDir, fSpeed)))
    {
        Safe_Release(pBulletTrail);
        MSG_BOX("CBulletTrail Create Failed");
        return nullptr;
    }

    return pBulletTrail;
}

void CBulletTrail::Free()
{
    CGameObject::Free();
}
