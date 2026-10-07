#include "pch.h"
#include "CBullet_Trail.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include <ctime>

CBullet_Trail::CBullet_Trail(LPDIRECT3DDEVICE9 pGraphicDev)
    : CTrail(pGraphicDev)
{
}

CBullet_Trail::~CBullet_Trail()
{
}

HRESULT CBullet_Trail::Ready_GameObject()
{
    m_fLifeTime = 10.f;

    m_vTrailPoint[0] = { 0.f,0.f,0.f };
    m_vTrailPoint[1] = { 0.f,0.05f,0.f };
    m_vTrailPoint[2] = { 0.f,0.f,-0.8f };
    m_vTrailPoint[3] = { 0.f,-0.05f,0.f };


    for (int i = 0; i < 4; ++i)
    {
        m_eColor[i] = { 1.f, 1.f, 1.f, 0.4f };
    }

    CTrail::Ready_GameObject();
    if (FAILED(Add_Component()))
        return E_FAIL;

    _vec3 vPos, vScale;
    static_cast<CProjectile*>(m_pBullet)->Get_Transform()->Get_Info(INFO_POS, &vPos);

    m_pTransformCom->Set_Pos(vPos);

    return S_OK;
}

_int CBullet_Trail::Update_GameObject(_float fTimeDelta)
{
    _int    iExit = CTrail::Update_GameObject(fTimeDelta);

    _vec3 vLook = m_pBullet->Get_Projectile_Dir();
    _vec3 vUp(0.f, 1.f, 0.f);
    _vec3 vRight;
    _vec3 vPos;

    D3DXVec3Normalize(&vLook, &vLook);

    D3DXVec3Cross(&vRight, &vUp, &vLook);
    D3DXVec3Normalize(&vRight, &vRight);

    D3DXVec3Cross(&vUp, &vLook, &vRight);
    D3DXVec3Normalize(&vUp, &vUp);

    _matrix * pWorld = m_pTransformCom->Get_World();

    static_cast<CProjectile*>(m_pBullet)->Get_Transform()->Get_Info(INFO_POS, &vPos);

    memcpy(&pWorld->m[0][0], &vRight, sizeof(_vec3));
    memcpy(&pWorld->m[1][0], &vUp, sizeof(_vec3));
    memcpy(&pWorld->m[2][0], &vLook, sizeof(_vec3));
    memcpy(&pWorld->m[3][0], &vPos, sizeof(_vec3));

    return iExit;
}

void CBullet_Trail::LateUpdate_GameObject(_float fTimeDelta)
{
    CTrail::LateUpdate_GameObject(fTimeDelta);
}

void CBullet_Trail::Render_GameObject()
{

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetTexture(0, nullptr);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CBullet_Trail::Add_Component()
{
    return S_OK;
}

CBullet_Trail* CBullet_Trail::Create(LPDIRECT3DDEVICE9 pGraphicDev, CProjectile* pBullet)
{
    CBullet_Trail* pBullet_Trail = new CBullet_Trail(pGraphicDev);

    pBullet_Trail->Set_Bullet(pBullet);
    
    if (FAILED(pBullet_Trail->Ready_GameObject()))
    {
        Safe_Release(pBullet_Trail);
        MSG_BOX("CBullet_Trail Create Failed");
        return nullptr;
    }

    return pBullet_Trail;
}

void CBullet_Trail::Free()
{
    CTrail::Free();
}
