#include "pch.h"
#include "CChasm.h"
#include "Client_Enum.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CPlaneTex.h"
#include "CBoxCollider.h"
#include "CCollisionMgr.h"
#include "CPlayer.h"
#include "CPlayerMovement.h"
#include "CRoomLayer.h"
#include "CManagement.h"
#include "CStage.h"

CChasm::CChasm(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev) {}

HRESULT CChasm::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);
    m_pColliderCom->Set_IsTrigger(true);
    m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
    return S_OK;
}

void CChasm::Set_Placement(const _vec3& vFloorPos, _float fWidth, _float fDepth)
{
    fWidth = max(0.01f, fWidth);
    fDepth = max(0.01f, fDepth);

    m_pTransformCom->Set_Pos(vFloorPos + _vec3(0.f, 0.01f, 0.f));
    m_pTransformCom->Set_Scale(fWidth, 1.f, fDepth);
    m_pColliderCom->Set_Extents(fWidth * 0.5f, 0.1f, fDepth * 0.5f);
    m_pColliderCom->Set_DiffPos(_vec3(0.f, -0.1f, 0.f));
    m_bPlacementApplied = true;
}

_int CChasm::Update_GameObject(_float fTimeDelta)
{
    // 최초 실행 (자리 잡을 때)
    if (!m_bPlacementApplied)
    {
        _vec3 vFloorPos;
        m_pTransformCom->Get_Info(INFO_POS, &vFloorPos);
        const _vec3 vScale = m_pTransformCom->Get_Scale();
        Set_Placement(vFloorPos, vScale.x, vScale.z);
    }

    const _int iExit = CGameObject::Update_GameObject(fTimeDelta);
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
    CCollisionMgr::GetInstance()->Add_Collider(COLL_ROOMLOGIC, m_pColliderCom);
    return iExit;
}

void CChasm::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();
}

void CChasm::OnCollisionEnter(COLLINFO info)
{
    if (info.iOtherID != COLL_PLAYER || !info.pOtherCollider)
        return;

    // 플레이어가 충돌하면 플레이어는 처음 위치로 돌아옴
    auto* pPlayer = dynamic_cast<CPlayer*>(info.pOtherCollider->Get_Owner());
    auto* pRoom = dynamic_cast<CRoomLayer*>(m_pOwner);
    if (!pPlayer || !pPlayer->GetTransform() || !pRoom)
        return;

    // 방 중심을 기준으로 한 복귀 위치. 이 값으로 방 안의 위치를 조절합니다.
    const _vec3 vReturnOffset(5.5f, 0.f, 0.f);
    const _vec3 vReturnPos = pRoom->GetCenterPos() + vReturnOffset;

    if (auto* pMovement = pPlayer->GetMovement())
        pMovement->Stop();

    pPlayer->GetTransform()->Set_Pos(vReturnPos);
    pPlayer->GetTransform()->Update_Component(0.f);
    info.pOtherCollider->LateUpdate_Component();

    if (auto* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene()))
        pStage->UpdatePlayerPosition(vReturnPos);
}

void CChasm::OnCollisionStay(COLLINFO info)
{
    if (info.iOtherID != COLL_PLAYER) return;
}

void CChasm::OnCollisionExit(COLLINFO info)
{
    if (info.iOtherID != COLL_PLAYER) return;
}

HRESULT CChasm::Add_Component()
{
    m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    if (!m_pTransformCom) return E_FAIL;
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", m_pTransformCom });

    m_pColliderCom = dynamic_cast<CBoxCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BoxCollider"));
    if (!m_pColliderCom) return E_FAIL;
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_BoxCollider", m_pColliderCom });

    m_pBufferCom = dynamic_cast<CPlaneTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PlaneTex"));
    if (!m_pBufferCom) return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", m_pBufferCom });

    m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Chasm_Texture"));
    if (!m_pTextureCom) return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

    return S_OK;
}

CChasm* CChasm::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    auto* pObject = new CChasm(pGraphicDev);
    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        return nullptr;
    }
    return pObject;
}

void CChasm::Free() { CGameObject::Free(); }
