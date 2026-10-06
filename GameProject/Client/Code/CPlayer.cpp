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

	return S_OK;
}

_int CPlayer::Update_GameObject(_float fTimeDelta)
{
	/* 캐릭터 타임스케일 */
	fTimeDelta *= CTimerMgr::GetInstance()->GetGroupTimeScale(TG_PLAYER);

    int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CCollisionMgr::GetInstance()->Add_Collider(COLL_PLAYER, m_pColliderCom);
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    if (m_bInputEnabled)
    {
	    UpdateInput();
    }

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

    if (m_fLeftInputDisabledTime > 0.f)
    {
        m_fLeftInputDisabledTime -= fTimeDelta;
        if (m_fLeftInputDisabledTime <= 0.f)
        {
            m_bInputEnabled = true;
            m_fLeftInputDisabledTime = 0.f;
        }
    }

    m_pAnimator->TransformPropagation(*m_pTransformCom->Get_World());

    CUIMgr::GetInstance()->Update_HPUI(m_iHP, m_bInvincible);

	return iExit;
}

void CPlayer::LateUpdate_GameObject(_float fTimeDelta)
{
    fTimeDelta *= CTimerMgr::GetInstance()->GetGroupTimeScale(TG_PLAYER);

    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Render_GameObject()
{
    /* 1인칭 시점일 때 */

    /* 3인칭 시점일 때 */
    if (0)
    {
        m_pTextureCom->Set_Texture(0);

        for (int i = 0; i < PP_END; ++i)
        {
            m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pBufferTransformCom[i]->Get_World());
            m_pBufferCom[i]->Render_Buffer();
        }
    }

    //m_pAnimator->RenderDebugTransform();
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

    /* Animator */
    array<TPlayerBuffer, PP_END> arrBuffer;
    array<wstring, PP_END> arrPartName = { L"Head", L"Body", L"LArm", L"RArm", L"LLeg", L"RLeg" };

    for (int i = 0; i < PP_END; ++i)
    {
        wstring wstrName = L"Proto_Player_" + arrPartName[i] + L"_Vertex";
        CPlayerPartTex* pBuffer = m_pBufferCom[i] = dynamic_cast<CPlayerPartTex*>(CProtoMgr::GetInstance()->Clone_Prototype(wstrName.c_str()));

        if (nullptr == pBuffer)
            return E_FAIL;

        wstrName = L"Com_Buffer_" + arrPartName[i];
        m_mapComponent[ID_STATIC].insert({ wstrName.c_str(), pBuffer });

        CTransform* pTransform = m_pBufferTransformCom[i] = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
        pTransform->SetUseLocal(true);

        if (nullptr == pTransform)
            return E_FAIL;

        wstrName = L"Com_BufferTransform_" + arrPartName[i];
        m_mapComponent[ID_DYNAMIC].insert({ wstrName.c_str(), pTransform });

        arrBuffer[i] = TPlayerBuffer{ pBuffer, pTransform };
    }

    m_pAnimator = CPlayerAnimator::Create(m_pGraphicDev);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_PlayerAnimator", m_pAnimator });

    m_pAnimator->SetBuffer(arrBuffer);

    /* Movement */
    m_pMovement = CPlayerMovement::Create(m_pGraphicDev);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_PlayerMovement", m_pMovement });
    m_pMovement->AttachTransform(m_pTransformCom);

    return S_OK;
}

void CPlayer::UpdateInput()
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
    if (D3DXVec2Length(&vCommand) > 1e-6) m_pAnimator->PlayLocomotion((m_pMovement->GetSprint()) ? EPlayerLocomotionState::SPRINT : EPlayerLocomotionState::WALK);
    else m_pAnimator->PlayLocomotion(EPlayerLocomotionState::IDLE);

    if (CDInputMgr::GetInstance()->Key_Press(DIK_SPACE))
    {
        m_pMovement->Jump();
    }

    UpdateWeaponInput();
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

    switch (tOutput.eWpEvent)
    {
    case EWeaponEvent::GUN_SHOT:
        m_pAnimator->PlayAction(EPlayerActionState::GUN_SHOOT);
        break;
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
    CUIMgr::GetInstance()->Update_HPUI(m_iHP, m_bInvincible);
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

void CPlayer::Revive()
{
    RestoreHP(m_iMaxHP);
    SetInputEnabled(true);
    m_pColliderCom->Set_IsActive(true);
}

void CPlayer::OnDead()
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

void CPlayer::RestoreHP(int iAmount)
{
    m_iHP += iAmount;
    m_iHP = clamp(m_iHP, 0, m_iMaxHP);
    
    CUIMgr::GetInstance()->Update_HPUI(m_iHP, false);
}

void CPlayer::SetInputEnabled(bool bFlag, float fDisabledTime)
{
    m_bInputEnabled = bFlag;
    if (bFlag == false && fDisabledTime >= 0.f)
    {
        m_fLeftInputDisabledTime = fDisabledTime;
    }
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
