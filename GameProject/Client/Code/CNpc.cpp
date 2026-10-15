#include "pch.h"
#include "CNpc.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CSphereCollider.h"
#include "CCollisionMgr.h"
#include "CClientCameraMgr.h"
#include "CCamera.h"
#include "CLayerContext.h"
#include "CRoomLayer.h"
#include "CSoundMgr.h"
#include "CManagement.h"
#include "CStage.h"
#include "CEnding.h"
#include "CIntro_MG1.h"

CNpc::CNpc(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CNpc::~CNpc()
{
}

HRESULT CNpc::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    _float fScale = 0.75f;
    _vec3 vScale = { fScale, fScale, fScale };
    m_pTransformCom->Set_Scale(vScale);

    auto* pRoomLayer = dynamic_cast<CRoomLayer*>(CLayerContext::GetLayer());

    if (pRoomLayer)
        pRoomLayer->m_OnRoomEvent.AddBinding(GetToken(), [this](const TRoomEventCtx& t) {OnRoomEvent(t); });

    return S_OK;
}

_int CNpc::Update_GameObject(_float fTimeDelta)
{
    if (m_bStart == false)
    {
        m_bStart = true;
        _vec3 vPos;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        _vec3 vScale = m_pTransformCom->Get_Scale();
        vPos.y += vScale.y;
        m_pTransformCom->Set_Pos(vPos);
    }
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);


    //Ending
    CTransform* pPlayerTransformCom = static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC,
        L"GameLogic_Layer", L"Player", L"Com_Transform"));
    _vec3 vPlayerPos;  pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);
    _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
    _vec3 vDist = vPlayerPos - vPos;
    if (D3DXVec3Length(&vDist) < 1.5f)
    {
        if (CStage* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->Get_Scene(0)))
        {
            pStage->Enter_Ending();
        }
    }
    
    return iExit;
}

void CNpc::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
    BillBoard();
}

void CNpc::BillBoard()
{
    auto* pCamera = CClientCameraMgr::GetInstance()->Get_ActiveCamera();
    if (!pCamera) 
        return;

    _matrix matCamera;
    pCamera->GetWorld(&matCamera);

    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    const _vec3 vDist = vPos - _vec3{ matCamera._41, matCamera._42, matCamera._43 };

    if (vDist.x * vDist.x + vDist.z * vDist.z < 0.000001f)
        return;

    m_pTransformCom->Set_Rotation_Raw({ 0.f, D3DXToDegree(atan2f(vDist.x, vDist.z)), 0.f });
}

void CNpc::OnRoomEvent(const TRoomEventCtx& t)
{
    switch (t.eType)
    {
    case ERoomEventType::ROOM_BEGIN:
    {
        if (m_bRoomBeginHandled)
            return;

        m_bRoomBeginHandled = true;

        // 구출과 관련된 음악 추가
        Engine::TBGMTrack tTrack{};
        tTrack.wstrID = L"Rescue";
        tTrack.wstrIntro = L"";
        tTrack.wstrLoop = L"Rescue.wav";

        CSoundMgr::GetInstance()->PlayBGM(tTrack);

        // 여기서 NPC의 구조 요청이나 대화 연출 시작
        if (auto* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene()))
            pStage->StartNpcDialogue();

        break;
    }
    default:
        break;
    }
}

void CNpc::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture(0);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CNpc::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Npc_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // RcTex
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });


    return S_OK;
}

void CNpc::OnCollisionEnter(COLLINFO eCollInfo)
{

}

CNpc* CNpc::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CNpc* pNpc = new CNpc(pGraphicDev);

    if (FAILED(pNpc->Ready_GameObject()))
    {
        Safe_Release(pNpc);
        MSG_BOX("CNpc Create Failed");
        return nullptr;
    }

    return pNpc;
}

void CNpc::Free()
{
    CGameObject::Free();
}
