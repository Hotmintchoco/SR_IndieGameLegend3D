#include "pch.h"
#include "CDdokddak.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CBoxCollider.h"
#include "CCollisionMgr.h"
#include "Client_Enum.h"
#include "CRoomLayer.h"
#include "CPlayer.h"
#include "CCollider.h"
#include "CSoundMgr.h"
#include "CManagement.h"
#include "CStage.h"

CDdokddak::CDdokddak(LPDIRECT3DDEVICE9 pGraphicDev, EDirection eDir)
    :CGameObject(pGraphicDev), m_eInitDir(eDir)
{
}

CDdokddak::~CDdokddak()
{
}

HRESULT CDdokddak::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    switch (m_eInitDir)
    {
    case EDirection::EAST:
        m_vDir = _vec3{1.f, 0.f, 0.f};
        break;
    case EDirection::WEST:
        m_vDir = _vec3{-1.f, 0.f, 0.f};
        break;
    case EDirection::SOUTH:
        m_vDir = _vec3{0.f, 0.f, -1.f};
        break;
    case EDirection::NORTH:
        m_vDir = _vec3{0.f, 0.f, 1.f};
        break;
    default:
        assert(0);
        break;
    }

    m_pTransformCom->Set_Scale(_vec3{ 0.5f, 0.5f, 0.5f });

    m_pColliderCom->Set_Extents(_vec3{ 0.5, 0.5, 0.5 });

    return S_OK;
}

_int CDdokddak::Update_GameObject(_float fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);
    CCollisionMgr::GetInstance()->Add_Collider(COLL_MONSTER, m_pColliderCom);

    m_bSkipCurrentFrameCollision = false;
    m_pTransformCom->Move_Pos(&m_vDir, m_fSpeed, fTimeDelta);

    return iExit;
}

void CDdokddak::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    BillBoard();
}

void CDdokddak::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(0);

    m_pBufferCom->Render_Buffer();
}

void CDdokddak::OnCollisionEnter(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    switch (iOtherID)
    {
    case COLLISIONID::COLL_OBSTACLE:
    {
        if (m_bSkipCurrentFrameCollision) return;
        m_vDir = -m_vDir;
        CStage* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
        if (pStage->GetCurrentRoomIndex() == static_cast<CRoomLayer*>(m_pOwner)->GetIndexFlat())
        {
            CSoundMgr::GetInstance()->PlaySFX(L"sfxslider.wav");
        }
        m_bSkipCurrentFrameCollision = true;

        break;
    }
    case COLLISIONID::COLL_PLAYER:
    {
        static_cast<CPlayer*>(pOtherCol->Get_Owner())->Hit(nullptr);
        break;
    }
    default:
        break;
    }
}

HRESULT CDdokddak::Add_Component()
{
    CComponent* pComponent = nullptr;

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

    // Buffer
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", pComponent });
    
    // Collider
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Ddokddak_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });


    return S_OK;
}

void CDdokddak::BillBoard()
{
    _vec3 vPlayerPos, vItemPos;
    CTransform* pPlayerTransform = static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
    pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
    m_pTransformCom->Get_Info(INFO_POS, &vItemPos);
    _vec3 vDist = vItemPos - vPlayerPos;

    float fYaw = atan2f(vDist.x, vDist.z);

    m_pTransformCom->Set_Rotation_Raw(_vec3{ 0.f, D3DXToDegree(fYaw), 0.f });
}

CDdokddak* CDdokddak::Create(LPDIRECT3DDEVICE9 pGraphicDev, EDirection eDir)
{
    CDdokddak* pObject = new CDdokddak(pGraphicDev, eDir);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CDdokddak Create Failed");
        return nullptr;
    }

    return pObject;
}

void CDdokddak::Free()
{
    CGameObject::Free();
}
