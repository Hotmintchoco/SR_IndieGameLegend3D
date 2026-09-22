#include "pch.h"
#include "CBullet.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CSphereCollider.h"
#include "CCollisionMgr.h"
#include "CManagement.h"
#include "CGun.h"
#include "CCollider.h"
#include "CBulletParticle.h"

CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_vDir(0.f, 0.f, 0.f), m_iBulletID(BULLET_DEFAULT), m_iBulletDmg(10), m_fBulletLife(0.f), m_fSpeed(50.f)
{
}


CBullet::~CBullet()
{
}

HRESULT CBullet::Ready_GameObject(const _vec3* pPos, const _vec3* pDir)
{
    m_iBulletID = static_cast<CGun*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"Gun"))->m_iCurBullet;
    m_iBulletDmg = static_cast<CGun*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"Gun"))->m_iDmg;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pTransformCom->Set_Pos(pPos->x, pPos->y, pPos->z);

    switch (m_iBulletID)
    {
    case BULLET_DEFAULT : 
        m_pTransformCom->m_vScale = { 0.3f, 0.3f, 0.3f };
        break;
    case BULLET_SMALL : 
        m_pTransformCom->m_vScale = { 0.15f, 0.15f, 0.15f };
        break;
    }
    D3DXVec3Normalize(&m_vDir, pDir);

	__super::Ready_GameObject();
    switch (m_iBulletID)
    {
    case BULLET_DEFAULT:
        m_pColliderCom->Set_CollisionID(COLL_PBULLET_NORMAL);
        break;
    case BULLET_SMALL:
        m_pColliderCom->Set_CollisionID(COLL_PBULLET_SMALL);
        break;
    }


    switch (m_iBulletID)
    {
    case BULLET_DEFAULT:
        m_pColliderCom->Set_Radius(0.4f);
        break;
    case BULLET_SMALL:
        m_pColliderCom->Set_Radius(0.2f);
        break;
    }
    

    m_pBulletTrail = CBulletTrail::Create(m_pGraphicDev, pPos, &m_vDir, m_fSpeed);
    CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(L"BulletTrail", m_pBulletTrail);

    return S_OK;
}

_int CBullet::Update_GameObject(const _float& fTimeDelta)
{

    m_fBulletLife += fTimeDelta;
    
    _float fBulletLife = 1.5f;
    if (m_fBulletLife >= fBulletLife)
    {
        Set_Dead(TRUE);
        m_pBulletTrail->Set_Dead(TRUE);

        _vec3 vPos, vRight, vUp, vLook;

        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);
        m_pTransformCom->Get_Info(INFO_UP, &vUp);
        m_pTransformCom->Get_Info(INFO_LOOK, &vLook);


        CBulletParticle* pParticle = CBulletParticle::Create(m_pGraphicDev, &vPos, &vRight, 0);
        CBulletParticle* pParticle1 = CBulletParticle::Create(m_pGraphicDev, &vPos, &vUp, 1);
        CBulletParticle* pParticle2 = CBulletParticle::Create(m_pGraphicDev, &vPos, &vLook, 2);

        CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(L"BulletParticle", pParticle);
        CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(L"BulletParticle", pParticle1);
        CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(L"BulletParticle", pParticle2);


        return NULL;
    }
    if (m_fBulletLife != fTimeDelta)
    {
        m_pTransformCom->Move_Pos(&m_vDir, m_fSpeed, fTimeDelta);
    }

    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);


    return iExit;
}

void CBullet::LateUpdate_GameObject(const _float& fTimeDelta)
{
    _vec3       vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);

    CGameObject::Compute_ViewZ(&vPos);


    _matrix matBill, matWorld, matView;

    D3DXMatrixIdentity(&matBill);

    matWorld = *m_pTransformCom->Get_World();

    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

    matBill._11 = matView._11;
    matBill._12 = matView._12;
    matBill._13 = matView._13;
    matBill._21 = matView._21;
    matBill._22 = matView._22;
    matBill._23 = matView._23;
    matBill._31 = matView._31;
    matBill._32 = matView._32;
    matBill._33 = matView._33;

    D3DXMatrixInverse(&matBill, 0, &matBill);

    matWorld = matBill * matWorld;

    m_pTransformCom->Set_World(&matWorld);

    // 충돌 매니저에 콜라이더 등록

    switch (m_iBulletID)
    {
    case BULLET_DEFAULT:
        CCollisionMgr::GetInstance()->Add_Collider(COLL_PBULLET_NORMAL, m_pColliderCom);
        break;
    case BULLET_SMALL:
        CCollisionMgr::GetInstance()->Add_Collider(COLL_PBULLET_SMALL, m_pColliderCom);
        break;
    }
    
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CBullet::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture(0);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CBullet::OnCollisionEnter(CGameObject* pOther)
{
	m_pColliderCom->Set_IsActive(false);
    Set_Dead(true);
}

HRESULT CBullet::Add_Component()
{
    CComponent* pComponent = nullptr;

    // RcCol
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    wstring pTextureKey;
    switch (m_iBulletID)
    {
    case BULLET_DEFAULT:
        pTextureKey = L"Proto_BulletTexture";
        break;
    case BULLET_SMALL:
        pTextureKey = L"Proto_BulletTexture_Small";
        break;
    }

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(pTextureKey.c_str()));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SphereCollider"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });


    return S_OK;
}


CBullet* CBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pPos, const _vec3* pDir)
{
    CBullet* pBullet = new CBullet(pGraphicDev);

    if (FAILED(pBullet->Ready_GameObject(pPos, pDir)))
    {
        Safe_Release(pBullet);
        MSG_BOX("CBullet Create Failed");
        return nullptr;
    }

    return pBullet;
}

void CBullet::Free()
{
    CGameObject::Free();
}
