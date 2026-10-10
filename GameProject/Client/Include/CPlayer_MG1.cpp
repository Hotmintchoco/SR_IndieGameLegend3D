#include "pch.h"
#include "CPlayer_MG1.h"
#include "CProtoMgr.h"
#include "CTimerMgr.h"
#include "CStage.h"
#include "CManagement.h"
#include "CPlayerAnimator.h"
#include "CMovement.h"
#include "CPlayerPartTex.h"
#include "CDInputMgr.h"
#include "CPlayerMovement.h"
#include "CCursorPolicyMgr.h"
#include "CUIMgr.h"
#include "CCollisionMgr.h"
#include "CWeaponSystem.h"
#include "Client_Struct.h"
#include "CClientCameraMgr.h"
#include "CCamera_MG1.h"
#include "CCamera2_MG1.h"
#include "CBox_MG1.h"

CPlayer_MG1::CPlayer_MG1(LPDIRECT3DDEVICE9 pGraphicDev)
    :CGameObject(pGraphicDev)
{
}

CPlayer_MG1::~CPlayer_MG1()
{
}

HRESULT CPlayer_MG1::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    m_pColliderCom->Set_Radius(m_fColliderScale);

    m_pColliderCom->Set_CollisionID(COLL_PLAYER);

    _vec3 vPos{ 0.f,0.f,0.f };
    m_pTransformCom->Set_Pos(vPos);

    _vec3 vScale{ 0.25f,0.25f,0.25f };
    m_pTransformCom->Set_Scale(vScale);
    return S_OK;
}

_int CPlayer_MG1::Update_GameObject(_float fTimeDelta)
{

    int iExit = CGameObject::Update_GameObject(fTimeDelta);


    CCollisionMgr::GetInstance()->Add_Collider(COLL_PLAYER, m_pColliderCom);
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);

    if (m_bInputEnabled)
    {
        Update_Input(fTimeDelta);
    }
    Check_Connection();
    //CUIMgr::GetInstance()->Update_HPUI(m_iHP, m_bInvincible);

    return iExit;
}

void CPlayer_MG1::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CPlayer_MG1::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture(0);
    m_pBufferCom2->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    
    //m_pTextureCom->Set_Texture(0);
	//for (int i = 0; i < PP_END; ++i)
	//{
	//	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pBufferTransformCom[i]->Get_World());
	//	m_pBufferCom[i]->Render_Buffer();
	//}
}

HRESULT CPlayer_MG1::Add_Component()
{
    CComponent* pComponent = nullptr;

    // 임시 RcCol ///////////////////
    pComponent = m_pBufferCom2 = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));
    if (nullptr == pComponent) return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });
    // 임시 //////////////////////////
    
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

    // Texture
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Player_Texture"));
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_speyederTexture"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    ///* Animator */
    //array<TPlayerBuffer, PP_END> arrBuffer;
    //array<wstring, PP_END> arrPartName = { L"Head", L"Body", L"LArm", L"RArm", L"LLeg", L"RLeg" };

    //for (int i = 0; i < PP_END; ++i)
    //{
    //    wstring wstrName = L"Proto_Player_" + arrPartName[i] + L"_Vertex";
    //    CPlayerPartTex* pBuffer = m_pBufferCom[i] = dynamic_cast<CPlayerPartTex*>(CProtoMgr::GetInstance()->Clone_Prototype(wstrName.c_str()));

    //    if (nullptr == pBuffer)
    //        return E_FAIL;

    //    wstrName = L"Com_Buffer_" + arrPartName[i];
    //    m_mapComponent[ID_STATIC].insert({ wstrName.c_str(), pBuffer });

    //    CTransform* pTransform = m_pBufferTransformCom[i] = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    //    if (nullptr == pTransform)
    //        return E_FAIL;

    //    pTransform->SetUseLocal(true);

    //    wstrName = L"Com_BufferTransform_" + arrPartName[i];
    //    m_mapComponent[ID_DYNAMIC].insert({ wstrName.c_str(), pTransform });

    //    arrBuffer[i] = TPlayerBuffer{ pBuffer, pTransform };
    //}

    //m_pVisualRootTransform = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    //if (nullptr == m_pVisualRootTransform)
    //    return E_FAIL;
    //m_mapComponent[ID_DYNAMIC].insert({ L"Com_VisualRootTransform", m_pVisualRootTransform });

    //m_pAnimator = CPlayerAnimator::Create(m_pGraphicDev);
    //m_mapComponent[ID_DYNAMIC].insert({ L"Com_PlayerAnimator", m_pAnimator });

    //m_pAnimator->SetBuffer(arrBuffer);
    //m_pAnimator->m_OnActionFinished.AddBinding(GetToken(), [this](const EPlayerActionState& Ctx) { OnActionAnimationFinished(Ctx); });

    ///* 애니메이션 루트 트랜스폼 */
    //m_pAnimRootTransform = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    //if (nullptr == m_pAnimRootTransform)
    //    return E_FAIL;
    //m_mapComponent[ID_DYNAMIC].insert({ L"Com_AnimRootTransform", m_pAnimRootTransform });
    //m_pAnimator->SetRootTransform(m_pAnimRootTransform);

    ///* Movement */
    //m_pMovement = CPlayerMovement::Create(m_pGraphicDev);
    //m_mapComponent[ID_DYNAMIC].insert({ L"Com_PlayerMovement", m_pMovement });
    //m_pMovement->AttachTransform(m_pTransformCom);

    ///* Socket */
    //_matrix  matOffset;
    //D3DXQUATERNION q;
    //_vec3 pos(0.f, -0.3f, 0.f);

    //D3DXQuaternionRotationYawPitchRoll(&q, 0.f, D3DXToRadian(90.f), 0.f);
    //D3DXMatrixAffineTransformation(&matOffset, 1.f, nullptr, &q, &pos);

    return S_OK;
}

void CPlayer_MG1::Update_Input(const _float& fTimeDelta)
{
    if (CDInputMgr::GetInstance()->Key_Press(DIK_W))
    {
        _vec3 vDir; m_pTransformCom->Get_Info(INFO_LOOK, &vDir);
        D3DXVec3Normalize(&vDir, &vDir);
        m_pTransformCom->Move_Pos(&vDir, 4.f, fTimeDelta);
    }
    if (CDInputMgr::GetInstance()->Key_Press(DIK_S))
    {
        _vec3 vDir; m_pTransformCom->Get_Info(INFO_LOOK, &vDir);
        vDir *= -1.f;
        D3DXVec3Normalize(&vDir, &vDir);
        m_pTransformCom->Move_Pos(&vDir, 4.f, fTimeDelta);

    }
    if (CDInputMgr::GetInstance()->Key_Press(DIK_D))
    {
        _vec3 vDir; m_pTransformCom->Get_Info(INFO_RIGHT, &vDir);
        D3DXVec3Normalize(&vDir, &vDir);
        m_pTransformCom->Move_Pos(&vDir, 4.f, fTimeDelta);
    }
    if (CDInputMgr::GetInstance()->Key_Press(DIK_A))
    {
        _vec3 vDir; m_pTransformCom->Get_Info(INFO_RIGHT, &vDir);
        vDir *= -1.f;
        D3DXVec3Normalize(&vDir, &vDir);
        m_pTransformCom->Move_Pos(&vDir, 4.f, fTimeDelta);
        //_vec3 vAngle = m_pTransformCom->Get_Angle();
        //vAngle.y -= 180.f * fTimeDelta;
        //m_pTransformCom->Set_Angle(vAngle);
    }

    if (CDInputMgr::GetInstance()->Key_Down(DIK_1))
    {
        m_iCameraType = 1;
        m_pCamera1->Set_IsActive(true);
        m_pCamera2->Set_IsActive(false);
    }
    else if (CDInputMgr::GetInstance()->Key_Down(DIK_2))
    {
        m_iCameraType = 2;
        m_pCamera1->Set_IsActive(false);
        m_pCamera2->Set_IsActive(true);
        static_cast<CCamera2_MG1*>(m_pCamera2)->Set_CameraActive(m_pTransformCom);
    }

    if (m_iCameraType == 2)return;

    if (!CCursorPolicyMgr::GetInstance()->IsCursorFixed()) return;

    _float fMinPitch = D3DXToRadian(-89.f);
    _float fMaxPitch = D3DXToRadian(89.f);

    const _long mouseX = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_X);
    const _long mouseY = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Y);

    _float fDeltaYaw = D3DXToRadian(mouseX / 10.f);
    _float fDeltaPitch = D3DXToRadian(mouseY / 10.f);

    if (!std::isfinite(fDeltaYaw) || !std::isfinite(fDeltaPitch)) return;
    m_fYaw = fmodf(m_fYaw + fDeltaYaw, 2.f * D3DX_PI);
    m_fPitch = max(fMinPitch, min(m_fPitch + fDeltaPitch, fMaxPitch));

    _vec3 fAngle = m_pTransformCom->Get_Angle();

    fAngle.x = D3DXToDegree(m_fPitch);
    fAngle.y = D3DXToDegree(m_fYaw);

    m_pTransformCom->Set_Angle(fAngle);

    //Update_LookFromAngles();

    //m_pMovement->Walk(vCommand);
    //if (D3DXVec2Length(&vCommand) > 1e-6)
    //{
    //    m_fInputYaw = atan2f(vCommand.x, vCommand.y);
    //    m_pAnimator->PlayLocomotion((m_pMovement->GetSprint()) ? EPlayerLocomotionState::SPRINT : EPlayerLocomotionState::WALK);
    //}
    //else
    //{
    //    m_pAnimator->PlayLocomotion(EPlayerLocomotionState::IDLE);
    //}

    //if (CDInputMgr::GetInstance()->Key_Press(DIK_SPACE))
    //{
    //    m_pMovement->Jump();
    //}
}

void CPlayer_MG1::UpdateInput()
{
    m_pMovement->SetSprint(CDInputMgr::GetInstance()->Key_Press(DIK_LSHIFT));

    _vec2 vCommand{ 0.f, 0.f }; // (x, z)
    if (CDInputMgr::GetInstance()->Key_Press(DIK_W))
    {
        vCommand += _vec2{ 0.f, 1.f };
        //_vec3 vDir = { 0.f, 0.f, 1.f };
        //m_pTransformCom->Move_Pos(&vDir, 1.f, 1.f / 500.f);
    }
    if (CDInputMgr::GetInstance()->Key_Press(DIK_S))
    {
        vCommand += _vec2{ 0.f, -1.f };
    }
    if (CDInputMgr::GetInstance()->Key_Press(DIK_D))
    {
        //vCommand += _vec2{ 1.f, 0.f };
        _vec3 vAngle = m_pTransformCom->Get_Angle();
        vAngle.y += 1.f;
        m_pTransformCom->Set_Angle(vAngle);
    }
    if (CDInputMgr::GetInstance()->Key_Press(DIK_A))
    {
        vCommand += _vec2{ -1.f, 0.f };
    }
    m_pMovement->Walk(vCommand);
    if (D3DXVec2Length(&vCommand) > 1e-6)
    {
        m_fInputYaw = atan2f(vCommand.x, vCommand.y);
        m_pAnimator->PlayLocomotion((m_pMovement->GetSprint()) ? EPlayerLocomotionState::SPRINT : EPlayerLocomotionState::WALK);
    }
    else
    {
        m_pAnimator->PlayLocomotion(EPlayerLocomotionState::IDLE);
    }

    if (CDInputMgr::GetInstance()->Key_Press(DIK_SPACE))
    {
        m_pMovement->Jump();
    }
}

void CPlayer_MG1::Check_Connection()
{
    if (m_iCameraType == 1)return;

    //CTransform* pBoxTransformCom1 = m_pBox1->Get_Component(ID_DYNAMIC, L"Com_Transform");

    CTransform* pBoxTransformCom1 = dynamic_cast<CTransform*>(m_pBox1->Get_Component(ID_DYNAMIC, L"Com_Transform"));
    CTransform* pBoxTransformCom2 = dynamic_cast<CTransform*>(m_pBox2->Get_Component(ID_DYNAMIC, L"Com_Transform"));


    //CTransform* pBoxTransformCom1 = dynamic_cast<CTransform*>(Get_Component(ID_DYNAMIC,
    //    L"GameLogic_Layer", L"Box1", L"Com_Transform"));
    //CTransform* pBoxTransformCom2 = dynamic_cast<CTransform*>(Get_Component(ID_DYNAMIC,
    //    L"GameLogic_Layer", L"Box1", L"Com_Transform"));

    //vPos2가 높음
    _vec3 vPos1, vPos2;
    pBoxTransformCom1->Get_Info(INFO_POS, &vPos1);
    pBoxTransformCom2->Get_Info(INFO_POS, &vPos2);

    
    _matrix matView;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    _vec3 vLook = { matView._13, matView._23, matView._33 };
    _vec3 vCameraPos = { matView._14, matView._24, matView._34 };

    D3DXVec3Normalize(&vLook, &vLook);

    _vec3 vDir = vCameraPos - vPos1;
    vDir.x /= vDir.y;
    vDir.y /= vDir.y;
    vDir.z /= vDir.y;
    vDir *= 0.5f;
    vPos1 += vDir;

    _vec3 vDist = vPos2 - vPos1;
    /*if (0.9f < D3DXVec3Length(&vDist) && D3DXVec3Length(&vDist) < 1.1f)
    {
        m_bConnection = true;
    }*/
    if (0.9f < D3DXVec3Length(&vDist) && D3DXVec3Length(&vDist) < 1.1f)
    {
        m_bConnection = true;
    }
    else
    {
        m_bConnection = false;

    }
}

void CPlayer_MG1::SyncCameraYaw()
{
    float fYaw = m_pCamera->Get_Yaw();
    m_pMovement->SetRefYaw(fYaw);

    float fVisualYaw = fYaw + (m_bInputYawIgnored ? 0.f : m_fInputYaw);
    m_pVisualRootTransform->Set_Rotation_Raw({ 0.f, D3DXToDegree(fVisualYaw), 0.f });
}

void CPlayer_MG1::OnActionAnimationFinished(const EPlayerActionState& Ctx)
{
    //switch (Ctx)
    //{
    //case EPlayerActionState::STRETCH_ARMS:
    //{
    //    m_bInputYawIgnored = false;
    //    CClientCameraMgr::GetInstance()->SetPlayerCameraMode(CAMERA_MODE::FIRST_PERSON);
    //    break;
    //}
    //}
}

void CPlayer_MG1::OnCollisionEnter(COLLINFO eCollInfo)
{
}

void CPlayer_MG1::OnCollisionStay(COLLINFO eCollInfo)
{
}

void CPlayer_MG1::OnHit(CGameObject* pSrcObj)
{
    if (m_bInvincible) return;

    --m_iHP;
    CUIMgr::GetInstance()->RequestHitEffect();

    /* 사망 시 빠지기 */
    if (m_iHP <= 0)
    {
        OnDead();
        return;
    }

    m_bInvincible = true;
    m_fLeftInvincibleTime = m_fInvincibleTime;

    if (!pSrcObj) return;

    CTransform* pSrcTransform = dynamic_cast<CTransform*>(pSrcObj->Get_Component(ID_DYNAMIC, L"Com_Transform"));
    if (!pSrcTransform) return;

    _vec3 vDist = m_pTransformCom->Get_Info_Value(INFO_POS) - pSrcTransform->Get_Info_Value(INFO_POS);
    m_pMovement->Knockback(vDist, 3.f);
    SetInputEnabled(false, 0.5f);
}

void CPlayer_MG1::Revive()
{
    RestoreHP(m_iMaxHP);
    SetInputEnabled(true);
    m_pColliderCom->Set_IsActive(true);
}

void CPlayer_MG1::OnDead()
{
    m_pMovement->Stop();
    SetInputEnabled(false);
    m_pColliderCom->Set_IsActive(false);
    /* 나중에 무기류도 비활성화 */

    CStage* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
    if (pStage)
    {
        pStage->OnPlayerDead();
    }
}

void CPlayer_MG1::RestoreHP(int iAmount)
{
    m_iHP += iAmount;
    m_iHP = clamp(m_iHP, 0, m_iMaxHP);

    CUIMgr::GetInstance()->Update_HPUI(m_iHP, false);
}

void CPlayer_MG1::SetInputEnabled(bool bFlag, float fDisabledTime)
{
    m_bInputEnabled = bFlag;
    if (bFlag == false && fDisabledTime >= 0.f)
    {
        m_fLeftInputDisabledTime = fDisabledTime;
    }
}

void CPlayer_MG1::SetPseudoScale(float fScale)
{
    m_pMovement->SetSpeedScale(fScale);
}

CPlayer_MG1* CPlayer_MG1::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CPlayer_MG1* pObject = new CPlayer_MG1(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CPlayer_MG1 Create Failed");
        return nullptr;
    }

    return pObject;
}

void CPlayer_MG1::Free()
{
    CGameObject::Free();
}
