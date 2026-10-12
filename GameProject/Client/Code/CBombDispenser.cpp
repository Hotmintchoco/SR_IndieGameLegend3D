#include "pch.h"
#include "CBombDispenser.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "Client_Enum.h"
#include "CManagement.h"
#include "CLiminalBomb.h"

CBombDispenser::CBombDispenser(LPDIRECT3DDEVICE9 pGraphicDev)
    :CGameObject(pGraphicDev)
{
}

CBombDispenser::~CBombDispenser()
{
}

HRESULT CBombDispenser::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
    m_pBufferCom->Set_Owner(this);

    return S_OK;
}

_int CBombDispenser::Update_GameObject(_float fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
    CCollisionMgr::GetInstance()->Add_Collider(COLL_OBSTACLE, m_pColliderCom);

    m_fTimeAfterSpawn += fTimeDelta;
    if (m_fTimeAfterSpawn >= m_fLaserSpawnInterval)
    {
        //m_pTransformCom->Get_Info_Value(INFO_POS), m_pTransformCom->Get_Info_Value(INFO_LOOK)
        // CLiminalBomb* pLaser = CLiminalBomb::Create(m_pGraphicDev);
        // if (pLaser)
        // {
        //     CManagement::GetInstance()->GetCurrentScene()->Add_GameObject(L"Laser", pLaser);
        // }
    }

    return iExit;
}

void CBombDispenser::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CBombDispenser::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture(100);
    m_pBufferCom->Render_Buffer();
}

void CBombDispenser::OnCollisionEnter(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    // switch (iOtherID)
    // {
    // default:
    //     break;
    // }
}


HRESULT CBombDispenser::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Buffer
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Cube_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Red_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CBoxCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BoxCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    return S_OK;
}

CBombDispenser* CBombDispenser::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CBombDispenser* pObject = new CBombDispenser(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CBombDispenser Create Failed");
        return nullptr;
    }

    return pObject;
}

void CBombDispenser::Free()
{
    CGameObject::Free();
}
