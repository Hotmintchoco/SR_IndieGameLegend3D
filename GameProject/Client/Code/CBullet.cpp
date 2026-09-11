#include "pch.h"
#include "CBullet.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_vDir(0.f, 0.f, 0.f)
{
}


CBullet::~CBullet()
{
}

HRESULT CBullet::Ready_GameObject(const _vec3* pPos, const _vec3* pDir)
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pTransformCom->Set_Pos(pPos->x, pPos->y, pPos->z);
    m_pTransformCom->m_vScale = { 0.2f, 0.2f, 0.2f };
    D3DXVec3Normalize(&m_vDir, pDir);

    return S_OK;
}

_int CBullet::Update_GameObject(const _float& fTimeDelta)
{
    _float fSpeed = 50.f;

    m_pTransformCom->Move_Pos(&m_vDir, fSpeed, fTimeDelta);

    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGrop(RENDER_ALPHA, this);


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
    matBill._13 = matView._13;
    matBill._31 = matView._31;
    matBill._33 = matView._33;

    D3DXMatrixInverse(&matBill, 0, &matBill);

    matWorld = matBill * matWorld;

    m_pTransformCom->Set_World(&matWorld);

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

HRESULT CBullet::Add_Component()
{
    CComponent* pComponent = nullptr;

    // RcCol
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BulletTexture"));

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
