#include "pch.h"
#include "CGameMachine.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CSphereCollider.h"
#include "CPlayer.h"
#include "CCollisionMgr.h"
#include "CRoomLayer.h"

CGameMachine::CGameMachine(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CGameMachine::~CGameMachine()
{
}

HRESULT CGameMachine::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pColliderCom->Set_Owner(this);
    m_pColliderCom->Set_Radius(1.f);

    return S_OK;
}

_int CGameMachine::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    CCollisionMgr::GetInstance()->Add_Collider(COLL_ROOMLOGIC, m_pColliderCom);

    return iExit;
}

void CGameMachine::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CGameMachine::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();
}

HRESULT CGameMachine::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Vertex
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_GameMachine_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_DownBuffer", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CSphereCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SphereCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_BoxCollider", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_GameMachine_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });


    return S_OK;
}

void CGameMachine::OnCollisionEnter(CGameObject* pOther)
{
    cout << "게임기 상호작용 가능" << endl;
}

CGameMachine* CGameMachine::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CGameMachine* pTriggerBox = new CGameMachine(pGraphicDev);

    if (FAILED(pTriggerBox->Ready_GameObject()))
    {
        Safe_Release(pTriggerBox);
        MSG_BOX("CGameMachine Create Failed");
        return nullptr;
    }

    return pTriggerBox;
}

void CGameMachine::Free()
{
    CGameObject::Free();
}
