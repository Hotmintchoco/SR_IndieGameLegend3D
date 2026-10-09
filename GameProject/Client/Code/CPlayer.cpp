#include "pch.h"
#include "CPlayer.h"
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
#include "CPlayerCamera.h"
#include "CSocket.h"
#include "CWeapon.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
    :CGameObject(pGraphicDev)
{
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;

	m_pColliderCom->Set_Radius(m_fColliderScale);
    m_pColliderCom->Set_DiffPos(_vec3{0.f, 0.5f, 0.f});
	m_pColliderCom->Set_CollisionID(COLL_PLAYER);

    CClientCameraMgr::GetInstance()->m_OnCameraViewChanged.AddBinding(GetToken(), [this](const CAMERA_MODE& Ctx) { OnCameraViewChanged(Ctx); });

    m_ePlayerCamMode = CAMERA_MODE::FIRST_PERSON;

	return S_OK;
}

_int CPlayer::Update_GameObject(_float fTimeDelta)
{
    /* 캐릭터 타임스케일 */
    fTimeDelta = CTimerMgr::GetInstance()->GetGroupTimeDelta(CTG_PLAYER);

    int iExit = CGameObject::Update_GameObject(fTimeDelta);

    SyncCameraYaw();

    CCollisionMgr::GetInstance()->Add_Collider(COLL_PLAYER, m_pColliderCom);
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    UpdateInput();

    CScene* pScene = CManagement::GetInstance()->GetCurrentScene();
    if (CStage* pStage = dynamic_cast<CStage*>(pScene))
    {
        pStage->UpdatePlayerPosition(m_pTransformCom->Get_Info_Value(INFO_POS));
    }

    if (m_bInvincible)
    {
        m_fLeftInvincibleTime -= fTimeDelta;
        if (m_fLeftInvincibleTime <= 0.f)
        {
            m_bInvincible = false;
        }
    }

    for (int i = 0; i < PIC_END; ++i)
    {
        if (m_fInputLockTime[i] > 0.f)
        {
            m_fInputLockTime[i] -= fTimeDelta;
        }
    }

    /* 업데이트 순서 : 애니메이션으로 캐릭터 위치 확정 후 소켓에 전달 */
    if (m_ePlayerCamMode == CAMERA_MODE::THIRD_PERSON)
    {
        _matrix matVisualWorld = (*m_pVisualRootTransform->Get_World()) * (*m_pTransformCom->Get_World());
        m_pAnimator->TransformPropagation(matVisualWorld);
        m_pLHandSocket->UpdateSocket();
        m_pRHandSocket->UpdateSocket();
    }
    /* ----------- */

    CUIMgr::GetInstance()->Update_HPUI(m_iHP, m_bInvincible);
 
	return iExit;
}

void CPlayer::LateUpdate_GameObject(_float fTimeDelta)
{
    fTimeDelta = CTimerMgr::GetInstance()->GetGroupTimeDelta(CTG_PLAYER);

    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Render_GameObject()
{
    switch (m_ePlayerCamMode)
    {
    /* 1인칭 시점일 때 */
    case CAMERA_MODE::FIRST_PERSON:
    {
        if (!m_pAnimator->IsFPPartVisible()) break;

        /* 카메라를 추적하는 1인칭 좌표계 업데이트 */
        _matrix matCamWorld;
        m_pCamera->GetWorld(&matCamWorld);
        m_pAnimator->TransformFPPropagation(matCamWorld);

        m_pTextureCom->Set_Texture(0);

        m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pFPBufferTransformCom[FP_LARM]->Get_World());
        m_pBufferCom[TP_LARM]->Render_Buffer();

        m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pFPBufferTransformCom[FP_RARM]->Get_World());
        m_pBufferCom[TP_RARM]->Render_Buffer();

        break;
    }
    /* 3인칭 시점일 때 */
    case CAMERA_MODE::THIRD_PERSON:
    {
        m_pTextureCom->Set_Texture(0);

        for (int i = 0; i < TP_END; ++i)
        {
            m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTPBufferTransformCom[i]->Get_World());
            m_pBufferCom[i]->Render_Buffer();
        }
        break;
    }
    default:
        break;
    }

    //m_pAnimator->RenderDebugTransform();
}

void CPlayer::SetWeaponSystem(CWeaponSystem* pSystem)
{
    m_pWeaponSystem = pSystem;
    m_pRHandSocket->SetTarget(m_pWeaponSystem->GetCurrentWeapon()->GetTransform());
}

CSocket* CPlayer::GetSocket(const wstring& wstrName)
{
    return dynamic_cast<CSocket*>(m_mapComponent[ID_DYNAMIC].at(wstrName));
}

HRESULT CPlayer::Add_Component()
{
    CComponent* pComponent = nullptr;

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
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Player_Texture"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    /* Animator(3인칭) */
    array<TPlayerBuffer, TP_END> arrBuffer;
    array<wstring, TP_END> arrPartName = { L"Head", L"Body", L"LArm", L"RArm", L"LLeg", L"RLeg" };

    for (int i = 0; i < TP_END; ++i)
    {
        wstring wstrName = L"Proto_Player_" + arrPartName[i] + L"_Vertex";
        CPlayerPartTex* pBuffer = m_pBufferCom[i] = dynamic_cast<CPlayerPartTex*>(CProtoMgr::GetInstance()->Clone_Prototype(wstrName.c_str()));

        if (nullptr == pBuffer)
            return E_FAIL;

        wstrName = L"Com_Buffer_" + arrPartName[i];
        m_mapComponent[ID_STATIC].insert({ wstrName.c_str(), pBuffer });

        CTransform* pTransform = m_pTPBufferTransformCom[i] = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

        if (nullptr == pTransform)
            return E_FAIL;

        pTransform->SetUseLocal(true);

        wstrName = L"Com_TPBufferTransform_" + arrPartName[i];
        m_mapComponent[ID_DYNAMIC].insert({ wstrName.c_str(), pTransform });

        arrBuffer[i] = TPlayerBuffer{ pBuffer, pTransform };
    }

    /* Animator(1인칭) */
    array<wstring, FP_END> arrFPPartName = { L"LArm", L"RArm" };
    array<CTransform*, FP_END> arrFPTransform;
    for (int i = 0; i < 2; ++i)
    {
        CTransform* pTransform = m_pFPBufferTransformCom[i] = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

        if (nullptr == pTransform)
            return E_FAIL;

        pTransform->SetUseLocal(true);

        wstring wstrName = L"Com_FPBufferTransform_" + arrFPPartName[i];
        m_mapComponent[ID_DYNAMIC].insert({ wstrName.c_str(), pTransform });
        
        arrFPTransform[i] = pTransform;
    }

    m_pVisualRootTransform = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    if (nullptr == m_pVisualRootTransform)
        return E_FAIL;
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_VisualRootTransform", m_pVisualRootTransform });

    m_pAnimator = CPlayerAnimator::Create(m_pGraphicDev);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_PlayerAnimator", m_pAnimator });

    m_pAnimator->SetBuffer(arrBuffer);
    m_pAnimator->SetFPTransform(arrFPTransform);
    m_pAnimator->m_OnActionFinished.AddBinding(GetToken(), [this](const EPlayerActionState& Ctx) { OnActionAnimationFinished(Ctx); });

    /* 애니메이션 루트 트랜스폼 */
    m_pAnimRootTransform = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    if (nullptr == m_pAnimRootTransform)
        return E_FAIL;
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_AnimRootTransform", m_pAnimRootTransform });
    m_pAnimator->SetRootTransform(m_pAnimRootTransform);

    /* Movement */
    m_pMovement = CPlayerMovement::Create(m_pGraphicDev);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_PlayerMovement", m_pMovement });
    m_pMovement->AttachTransform(m_pTransformCom);

    /* Socket */
    _matrix  matOffset;
    D3DXQUATERNION q;
    _vec3 pos(0.f, -0.3f, 0.f);

    D3DXQuaternionRotationYawPitchRoll(&q, 0.f, D3DXToRadian(90.f), 0.f);
    D3DXMatrixAffineTransformation(&matOffset, 1.f, nullptr, &q, &pos);

    m_pLHandSocket = CSocket::Create(m_pGraphicDev, m_pTPBufferTransformCom[TP_LARM]);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Socket_LHand", m_pLHandSocket });
    m_pLHandSocket->SetOffset(matOffset);

    m_pRHandSocket = CSocket::Create(m_pGraphicDev, m_pTPBufferTransformCom[TP_RARM]);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Socket_RHand", m_pRHandSocket });
    m_pRHandSocket->SetOffset(matOffset);


    return S_OK;
}

void CPlayer::UpdateInput()
{
    if (IsInputAllowed(PIC_MOVE))
    {
        m_pMovement->SetSprint(CDInputMgr::GetInstance()->Key_Press(DIK_LSHIFT));

        _vec2 vCommand{ 0.f, 0.f }; // (x, z)
        if (CDInputMgr::GetInstance()->Key_Press(DIK_W))
        {
            vCommand += _vec2{ 0.f, 1.f };
        }
        if (CDInputMgr::GetInstance()->Key_Press(DIK_S))
        {
            vCommand += _vec2{ 0.f, -1.f };
        }
        if (CDInputMgr::GetInstance()->Key_Press(DIK_D))
        {
            vCommand += _vec2{ 1.f, 0.f };
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

    if (IsInputAllowed(PIC_WEAPON))
    {
        UpdateWeaponInput();
    }
}

void CPlayer::UpdateWeaponInput()
{
    /* 무기 입력 처리 */
    auto* pInput = CDInputMgr::GetInstance();
    bool bCursorFixed = CCursorPolicyMgr::GetInstance()->IsCursorFixed();
    const pair<EWeaponAction, MOUSEKEYSTATE> WeaponActionMapping[] = {
        { EWeaponAction::Primary, DIM_LB },
        { EWeaponAction::Secondary, DIM_RB },
    };

    TWeaponSystemInput tSysInput{};

    for (auto& [eAction, eKey] : WeaponActionMapping)
    {
        TWeaponInput tWpInput{};

        tWpInput.eAction = eAction;

        if (!bCursorFixed)                  tWpInput.eState = EInputState::NONE;
        else if (pInput->Mouse_Down(eKey))  tWpInput.eState = EInputState::Pressed;
        else if (pInput->Mouse_Press(eKey)) tWpInput.eState = EInputState::Held;
        else if (pInput->Mouse_Up(eKey))    tWpInput.eState = EInputState::Released;

        tSysInput.tWeaponInput[(int)eAction] = tWpInput;
    }

    tSysInput.bUltAttack = CDInputMgr::GetInstance()->Key_Down(DIK_C);
    tSysInput.bMove = pInput->Key_Press(DIK_W)
        || pInput->Key_Press(DIK_A)
        || pInput->Key_Press(DIK_S)
        || pInput->Key_Press(DIK_D);
    tSysInput.bSpecialSwitchPressed = pInput->Key_Down(DIK_F);
    tSysInput.bSprint = pInput->Key_Press(DIK_LSHIFT);
    tSysInput.bSwitchWeapon = pInput->Key_Down(DIK_Q);

    TWeaponSystemOutput tOutput = m_pWeaponSystem->UpdateInput(tSysInput);

    switch (tOutput.tWpOut.eWpEvent)
    {
    case EWeaponAnimEvent::GUN_SHOT:
        m_pAnimator->PlayAction(EPlayerActionState::GUN_SHOOT);
        break;
    case EWeaponAnimEvent::ULT_SHOTGUN:
        m_bInputYawIgnored = true;
        CClientCameraMgr::GetInstance()->SetPlayerCameraMode(CAMERA_MODE::THIRD_PERSON);
        m_pAnimator->PlayAction(EPlayerActionState::ULT_SHOTGUN);
        break;
    case EWeaponAnimEvent::ULT_RAPIDGUN:
        m_pAnimator->PlayAction(EPlayerActionState::ULT_RAPIDGUN_START);
        break;
    case EWeaponAnimEvent::ULT_LIMINALGUN_START:
        m_pAnimator->PlayAction(EPlayerActionState::ULT_LIMINALGUN_START);
        break;
    case EWeaponAnimEvent::ULT_LIMINALGUN_END:
        m_pAnimator->PlayAction(EPlayerActionState::ULT_LIMINALGUN_END);
        break;
    }
}

void CPlayer::OnCameraViewChanged(const CAMERA_MODE& Ctx)
{
    m_ePlayerCamMode = Ctx;
    m_pWeaponSystem->ApplyCameraView(m_ePlayerCamMode);
}

void CPlayer::SyncCameraYaw()
{
    float fYaw = m_pCamera->Get_Yaw();
    m_pMovement->SetRefYaw(fYaw);

    float fVisualYaw = fYaw + (m_bInputYawIgnored ? 0.f : m_fInputYaw);
    m_pVisualRootTransform->Set_Rotation_Raw({ 0.f, D3DXToDegree(fVisualYaw), 0.f });
}

void CPlayer::OnActionAnimationFinished(const EPlayerActionState& Ctx)
{
    switch (Ctx)
    {
    case EPlayerActionState::ULT_SHOTGUN:
    {
        m_bInputYawIgnored = false;
        CClientCameraMgr::GetInstance()->SetPlayerCameraMode(CAMERA_MODE::FIRST_PERSON);
        break;
    }
    }
}

void CPlayer::OnCollisionEnter(COLLINFO eCollInfo)
{
}

void CPlayer::OnCollisionStay(COLLINFO eCollInfo)
{
}

void CPlayer::OnHit(CGameObject* pSrcObj)
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
    LockInputFor(PIC_MOVE, 0.5f);
}

void CPlayer::Revive()
{
    RestoreHP(m_iMaxHP);
    UnlockInput(PIC_MOVE);
    UnlockInput(PIC_WEAPON);
    m_pColliderCom->Set_IsActive(true);
}

void CPlayer::OnDead()
{
    m_pMovement->Stop();
    LockInput(PIC_MOVE);
    LockInput(PIC_WEAPON);
    m_pColliderCom->Set_IsActive(false);
    /* 나중에 무기류도 비활성화 */

    CStage* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
    if (pStage)
    {
        pStage->OnPlayerDead();
    }
}

void CPlayer::RestoreHP(int iAmount)
{
    m_iHP += iAmount;
    m_iHP = clamp(m_iHP, 0, m_iMaxHP);
    
    CUIMgr::GetInstance()->Update_HPUI(m_iHP, false);
}

void CPlayer::LockInput(EPlayerInputChannel e)
{
    ++m_iInputLockCount[e];
}

void CPlayer::LockInputFor(EPlayerInputChannel e, float fDuration)
{
    m_fInputLockTime[e] = max(m_fInputLockTime[e], fDuration);
}

void CPlayer::UnlockInput(EPlayerInputChannel e)
{
    if (m_iInputLockCount[e] > 0) --m_iInputLockCount[e];
}

bool CPlayer::IsInputAllowed(EPlayerInputChannel e)
{
    return (m_iInputLockCount[e] == 0 && m_fInputLockTime[e] <= 0.f);
}

void CPlayer::SetPseudoScale(float fScale)
{
    m_pMovement->SetSpeedScale(fScale);
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CPlayer* pObject = new CPlayer(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CPlayer Create Failed");
        return nullptr;
    }

    return pObject;
}

void CPlayer::Free()
{
    CGameObject::Free();
}
