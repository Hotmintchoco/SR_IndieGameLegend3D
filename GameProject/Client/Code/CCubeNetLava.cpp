#include "pch.h"
#include "CCubeNetLava.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "CLiminalFrame.h"
#include "CPlayer.h"

CCubeNetLava::CCubeNetLava(LPDIRECT3DDEVICE9 pGraphicDev)
    : CCubeNet(pGraphicDev)
{
}

CCubeNetLava::~CCubeNetLava()
{
}

HRESULT CCubeNetLava::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CCubeNet::Ready_GameObject()))
        return E_FAIL;

    m_pColliderCom->Set_Extents(_vec3{ 0.25f, 0.25f, 0.25f });
    m_pColliderCom->Set_DiffPos(_vec3{ 0.0f, 0.5f, 0.0f });
    m_iAnimFrameCount = m_pTextureCom->GetCount();

    return S_OK;
}

_int CCubeNetLava::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CCubeNet::Update_GameObject(fTimeDelta);

    CCollisionMgr::GetInstance()->Add_Collider(COLL_ROOMLOGIC, m_pColliderCom);

    if (m_iAnimFrameCount > 0)
    {
        m_fAnimAccTime += fTimeDelta;
        while (m_fAnimAccTime >= m_fAnimFrameInterval)
        {
            m_fAnimAccTime -= m_fAnimFrameInterval;
            m_iCurrentAnimFrame = (m_iCurrentAnimFrame + 1) % m_iAnimFrameCount;
        }
    }

    return iExit;
}

void CCubeNetLava::LateUpdate_GameObject(_float fTimeDelta)
{
    CCubeNet::LateUpdate_GameObject(fTimeDelta);
}

void CCubeNetLava::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture(m_iCurrentAnimFrame);
    m_pBufferCom->Render_Buffer();
}

void CCubeNetLava::OnCollisionStay(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    switch (iOtherID)
    {
    case COLLISIONID::COLL_PLAYER:
    {
        CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOtherCol->Get_Owner());
        if (pPlayer) pPlayer->OnHit(this);
        break;
    }
    default:
        break;
    }
}

HRESULT CCubeNetLava::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CubeNet_Lava_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CCubeNetLava* CCubeNetLava::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CCubeNetLava* pObject = new CCubeNetLava(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CCubeNetLava Create Failed");
        return nullptr;
    }

    return pObject;
}

void CCubeNetLava::Free()
{
    CCubeNet::Free();
}