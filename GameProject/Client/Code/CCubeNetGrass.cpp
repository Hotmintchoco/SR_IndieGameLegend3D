#include "pch.h"
#include "CCubeNetGrass.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "CLiminalFrame.h"

CCubeNetGrass::CCubeNetGrass(LPDIRECT3DDEVICE9 pGraphicDev)
    : CCubeNet(pGraphicDev)
{
}

CCubeNetGrass::~CCubeNetGrass()
{
}

HRESULT CCubeNetGrass::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CCubeNet::Ready_GameObject()))
        return E_FAIL;

    m_pTransformCom->Rotation(ROT_X, 90.f);
    m_pColliderCom->Set_Extents(_vec3{ 0.5f, 0.5f, 0.5f });
    m_pColliderCom->Set_DiffPos(_vec3{ 0.0f, 0.5f, 0.0f });

    return S_OK;
}

_int CCubeNetGrass::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CCubeNet::Update_GameObject(fTimeDelta);

    CCollisionMgr::GetInstance()->Add_Collider(COLL_OBSTACLE, m_pColliderCom);

    return iExit;
}

void CCubeNetGrass::LateUpdate_GameObject(_float fTimeDelta)
{
    CCubeNet::LateUpdate_GameObject(fTimeDelta);
}

void CCubeNetGrass::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();
}

HRESULT CCubeNetGrass::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CubeNet_Grass_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CCubeNetGrass::OnCollisionStay(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    switch (iOtherID)
    {
    case COLLISIONID::COLL_PLAYER:
    {
        Obstacle_Collision(eCollInfo.pOtherCollider, m_pColliderCom);
        break;
    }
    default:
        break;
    }
}

CCubeNetGrass* CCubeNetGrass::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CCubeNetGrass* pObject = new CCubeNetGrass(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CCubeNetGrass Create Failed");
        return nullptr;
    }

    return pObject;
}

void CCubeNetGrass::Free()
{
    CCubeNet::Free();
}