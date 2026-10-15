#include "pch.h"
#include "CCubeNet.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "CLiminalFrame.h"

CCubeNet::CCubeNet(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CCubeNet::~CCubeNet()
{
}

HRESULT CCubeNet::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pBufferCom->Set_Owner(this);
    m_pColliderCom->Set_Owner(this);

    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    /* 직교 미니게임 0.5 격자에 맞춰 블럭 크기 절반 (콜라이더는 각 블럭에서 절반 크기로 설정) */
    m_pTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);

    return S_OK;
}

_int CCubeNet::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CCubeNet::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CCubeNet::Render_GameObject()
{
}

HRESULT CCubeNet::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Buffer
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CubeNet_Buffer"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CBoxCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BoxCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    return S_OK;
}

void CCubeNet::ComputeFlatBounds(_vec2& vMin, _vec2& vMax)
{
    const _vec3 vPos = m_pTransformCom->Get_Info_Value(INFO_POS);
    const _vec3 vScale = m_pTransformCom->Get_Scale();
    const _vec2 vHalf{ vScale.x * 0.5f, vScale.z * 0.5f };

    vMin = { vPos.x - vHalf.x, vPos.z - vHalf.y };
    vMax = { vPos.x + vHalf.x, vPos.z + vHalf.y };
}

CCubeNet* CCubeNet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CCubeNet* pObject = new CCubeNet(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CCubeNet Create Failed");
        return nullptr;
    }

    return pObject;
}

void CCubeNet::Free()
{
    CGameObject::Free();
}