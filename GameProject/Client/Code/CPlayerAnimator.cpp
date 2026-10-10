#include "pch.h"
#include "CPlayerAnimator.h"
#include "CTransform.h"
#include "CPlayerPartTex.h"
#include "CImGUITool.h"
#include "CClientCameraMgr.h"
#include "CPlayerCamera.h"

// enum PLAYERPART { PP_BODY, PP_HEAD, PP_LARM, PP_RARM, PP_LLEG, PP_RLEG, PP_END };
const array<int, TP_END> CPlayerAnimator::s_arrParent =
{ TP_ROOT, TP_BODY, TP_BODY, TP_BODY, TP_ROOT, TP_ROOT };

const array<PLAYERTPPART, TP_END> CPlayerAnimator::s_arrUpdateOrder =
{ TP_BODY, TP_LLEG, TP_RLEG, TP_HEAD, TP_LARM, TP_RARM };

const char* CPlayerAnimator::s_szPartName[TP_END] =
{ "Body", "Head", "L Arm", "R Arm", "L Leg", "R Leg" };

/* 1인칭 팔 애니메이션 용 키프레임 */
static const TFViewKey s_arrRapidGunStartKeys[] =
{   
    { 0.0f, { -0.330f, -0.425f, 0.460f }, { -195.0f, -6.5f, -80.5f } },
    { 0.3f, { -0.345f, -0.265f, 0.450f }, { -191.0f, 0.0f, -10.5f }, EEase::E_OUT },
    { 0.6f, { -0.330f, -0.425f, 0.460f }, { -195.0f, -6.5f, -80.5f }, EEase::E_IN },
};
static constexpr int s_iRapidGunStartKeyCount = _countof(s_arrRapidGunStartKeys);

static const TFViewKey s_arrLiminalGunStartKeys[] =
{
    { 0.00f, { -0.600f, -0.650f, 0.500f }, { -179.5f, 7.5f, -60.0f } },                    // 화면 아래 바깥
    { 0.20f, { -0.490f, -0.420f, 0.480f }, { -175.0f, 4.0f, -73.0f }, EEase::E_IN },        // 가속하며 올라옴
    { 0.40f, { -0.335f, -0.265f, 0.450f }, { -169.0f, 0.0f, -85.5f }, EEase::E_OUT }, 
};
static constexpr int s_iLiminalGunStartKeyCount = _countof(s_arrLiminalGunStartKeys);

static const TFViewKey s_arrLiminalGunLoopKeys[] =
{
    { 0.00f, { -0.335f, -0.265f, 0.450f }, { -169.0f,  0.0f, -85.5f }, EEase::E_INOUT },
    { 0.60f, { -0.335f, -0.258f, 0.452f }, { -170.0f,  3.5f, -92.0f }, EEase::E_INOUT },
    { 1.20f, { -0.335f, -0.265f, 0.450f }, { -169.0f,  0.0f, -85.5f }, EEase::E_INOUT },
};
static constexpr int s_iLiminalGunLoopKeyCount = _countof(s_arrLiminalGunLoopKeys);

static const TFViewKey s_arrLiminalGunEndKeys[] =
{
    { 0.00f, { -0.335f, -0.265f, 0.450f }, { -169.0f,  0.0f, -85.5f } },
    { 0.30f, { -0.330f, -0.425f, 0.460f }, { -195.0f, -6.5f, -80.5f }, EEase::E_IN },
};
static constexpr int s_iLiminalGunEndKeyCount = _countof(s_arrLiminalGunEndKeys);

/* 디버그 렌더용 */
struct TFViewDebugClip
{
    const char* szName;
    const TFViewKey* pKeys;
    int                iCount;
    EPlayerActionState eAction;
};

static const TFViewDebugClip s_arrFViewDebugClips[] =
{
    { "RapidGun Start",   s_arrRapidGunStartKeys,   s_iRapidGunStartKeyCount,   EPlayerActionState::ULT_RAPIDGUN_START   },
    { "LiminalGun Start", s_arrLiminalGunStartKeys, s_iLiminalGunStartKeyCount, EPlayerActionState::ULT_LIMINALGUN_START },
    { "LiminalGun Loop",  s_arrLiminalGunLoopKeys,  s_iLiminalGunLoopKeyCount,  EPlayerActionState::ULT_LIMINALGUN_LOOP  },
    { "LiminalGun End",   s_arrLiminalGunEndKeys,   s_iLiminalGunEndKeyCount,   EPlayerActionState::ULT_LIMINALGUN_END   },
};
static constexpr int s_iFViewDebugClipCount = _countof(s_arrFViewDebugClips);


/* 이징을 위한 헬퍼 함수 */
static float ApplyEase(EEase e, float t)
{
    switch (e)
    {
    case EEase::E_IN:     return t * t * t;
    case EEase::E_OUT: { float u = 1.f - t; return 1.f - u * u * u; }
    case EEase::E_INOUT: return t * t * (3.f - 2.f * t);   // smoothstep
    case EEase::E_LINEAR:
    default:            return t;
    }
}

CPlayerAnimator::CPlayerAnimator(LPDIRECT3DDEVICE9 pGraphicDev)
    : CComponent(pGraphicDev)
{
}

CPlayerAnimator::~CPlayerAnimator()
{
}

_int CPlayerAnimator::Update_Component(_float fTimeDelta)
{
    if (m_bAnimPause) return S_OK;

    m_tPose.Reset();   

    UpdateLocomotion(fTimeDelta);

    UpdateAction(fTimeDelta); /* 블렌딩으로 인해 OnAction 조건은 없음 */

    ApplyPose();

	return S_OK;
}

void CPlayerAnimator::LateUpdate_Component()
{
}

void CPlayerAnimator::PlayAction(EPlayerActionState eAction)
{
    m_eAction = eAction;
    m_fActionTime = 0.f;
}

void CPlayerAnimator::PlayLocomotion(EPlayerLocomotionState eLoco)
{
    if (m_eLoco == eLoco) return;
    
    m_eLoco = eLoco;
}

bool CPlayerAnimator::IsFPPartVisible()
{
    switch (m_eAction)
    {
    case EPlayerActionState::ULT_RAPIDGUN_START:
    case EPlayerActionState::ULT_LIMINALGUN_START:
    case EPlayerActionState::ULT_LIMINALGUN_LOOP:
    case EPlayerActionState::ULT_LIMINALGUN_END:
        return true;
        break;
    }

    return false;
}

void CPlayerAnimator::UpdateLocomotion(float fTimeDelta)
{
    const TLocoParam tTarget = GetLocoParam(m_eLoco);
    const float t = min(fTimeDelta * s_fLocoBlendSpeed, 1.f); // 1.f는 프레임 방어선

    m_tCurLoco.fFreq += (tTarget.fFreq - m_tCurLoco.fFreq) * t;
    m_tCurLoco.fLegAmp += (tTarget.fLegAmp - m_tCurLoco.fLegAmp) * t;
    m_tCurLoco.fArmAmp += (tTarget.fArmAmp - m_tCurLoco.fArmAmp) * t;

    m_fPhase += m_tCurLoco.fFreq * fTimeDelta;
    m_fPhase = fmodf(m_fPhase, D3DX_PI * 2.f);

    const float s = sinf(m_fPhase);
    const float fLeg = s_fForward * s * m_tCurLoco.fLegAmp;
    const float fArm = s_fForward * s * m_tCurLoco.fArmAmp;

    m_tPose.vRot[TP_LLEG].x += fLeg;
    m_tPose.vRot[TP_RLEG].x -= fLeg;
    m_tPose.vRot[TP_LARM].x -= fArm;
    m_tPose.vRot[TP_RARM].x += fArm;
}

void CPlayerAnimator::UpdateAction(float fTimeDelta)
{
    if (OnAction())
    {
        m_fActionTime += fTimeDelta;

        switch (m_eAction)
        {
        case EPlayerActionState::GUN_SHOOT:
            SampleFire(m_fActionTime);
            if (m_fActionTime >= s_tShootParam.fHoldTime)
                m_eAction = EPlayerActionState::NONE;   // 종료. m_tActionPose는 남겨서 페이드아웃에 사용
            break;
        case EPlayerActionState::ULT_SHOTGUN:
            SampleShotGunUltimate(m_fActionTime);
            if (m_fActionTime >= s_fRipperDuration)
            {
                m_pRootTransform->Set_Rotation_Raw({ 0.f, 0.f, 0.f });
                m_OnActionFinished.Broadcast(m_eAction);
                m_eAction = EPlayerActionState::NONE;
            }
            break;
        case EPlayerActionState::ULT_RAPIDGUN_START:
            SampleRapidGunUltimateStart(m_fActionTime);
            if (m_fActionTime >= 1.f)
                m_eAction = EPlayerActionState::NONE;
            break;
        case EPlayerActionState::ULT_LIMINALGUN_START:
            SampleLiminalGunUltimateStart(m_fActionTime);
            if (m_fActionTime >= 0.6f)
                PlayAction(EPlayerActionState::ULT_LIMINALGUN_LOOP);
            break;
        case EPlayerActionState::ULT_LIMINALGUN_LOOP:
            SampleLiminalGunUltimateLoop(m_fActionTime);
            if (m_fActionTime >= 1.2f)
                PlayAction(EPlayerActionState::ULT_LIMINALGUN_LOOP);
            break;
        case EPlayerActionState::ULT_LIMINALGUN_END:
            SampleLiminalGunUltimateEnd(m_fActionTime);
            if (m_fActionTime >= 1.0f)
                m_eAction = EPlayerActionState::NONE;
            break;
        default:
            m_eAction = EPlayerActionState::NONE;
            break;
        }
    }

    const float fTarget = OnAction() ? 1.f : 0.f;
    /* 감쇠 적용 : Action 중이면 1로, 아니면 0으로 */
    m_fActionWeight += (fTarget - m_fActionWeight) * (1.f - expf(-s_fActionBlendSpeed * fTimeDelta));

    if (m_fActionWeight < 1e-3f)
        return;

    /* 3. 마스크된 파츠만 로코모션 포즈 → 액션 포즈로 블렌드 */
    for (int i = 0; i < TP_END; ++i)
    {
        if (!m_tActionPose.bMask[i])
            continue;

        m_tPose.vRot[i] += (m_tActionPose.vRot[i] - m_tPose.vRot[i]) * m_fActionWeight;
    }
}

void CPlayerAnimator::SampleFire(float fTime)
{
    /* 반동 커브: 짧게 선형으로 차오른 뒤 지수 감쇠로 복귀 */
    const float fRecoil = (fTime < s_tShootParam.fAttack)
        ? fTime / s_tShootParam.fAttack
        : expf(-(fTime - s_tShootParam.fAttack) * s_tShootParam.fRecover);

    m_tActionPose.Reset();
    m_tActionPose.bMask[TP_RARM] = true;
    m_tActionPose.vRot[TP_RARM].x = s_fForward * (90.f + s_tShootParam.fKick * fRecoil);   // 정면 90도 + 위로 반동
}

void CPlayerAnimator::SampleShotGunUltimate(float fTime)
{
    float fRotationSpeed = 540.f;
    m_pRootTransform->Set_Rotation_Raw(_vec3{ 0.f, fRotationSpeed * fTime, 0.f });

    m_tActionPose.Reset();
    m_tActionPose.bMask[TP_LARM] = true;
    m_tActionPose.bMask[TP_RARM] = true;

    m_tActionPose.vRot[TP_LARM].z = -90.f;
    m_tActionPose.vRot[TP_RARM].z = 90.f;
}

void CPlayerAnimator::SampleKeys(const TFViewKey* pKeys, int iCount, float fTime, CTransform* pTarget)
{
    if (fTime <= pKeys[0].fTime) 
    { 
        pTarget->Set_Pos(pKeys[0].vCamLocal);
        pTarget->Set_Rotation_Raw(pKeys[0].vRotDegree);
        return;
    }
    if (fTime >= pKeys[iCount - 1].fTime)
    {
        pTarget->Set_Pos(pKeys[iCount - 1].vCamLocal);
        pTarget->Set_Rotation_Raw(pKeys[iCount - 1].vRotDegree);
        return;
    }

    int i = 0;
    while (fTime > pKeys[i + 1].fTime) ++i;

    const TFViewKey& k0 = pKeys[i];
    const TFViewKey& k1 = pKeys[i + 1];
    float a = (fTime - k0.fTime) / (k1.fTime - k0.fTime);
    a = ApplyEase(k1.eEase, a);

    pTarget->Set_Pos(k0.vCamLocal + (k1.vCamLocal - k0.vCamLocal) * a);
    pTarget->Set_Rotation_Raw(k0.vRotDegree + (k1.vRotDegree - k0.vRotDegree) * a);
}

void CPlayerAnimator::SampleRapidGunUltimateStart(float fTime)
{
    SampleKeys(s_arrRapidGunStartKeys, s_iRapidGunStartKeyCount, fTime, m_arrFPTransform[FP_LARM]);
}

void CPlayerAnimator::SampleLiminalGunUltimateStart(float fTime)
{
    SampleKeys(s_arrLiminalGunStartKeys, s_iLiminalGunStartKeyCount, fTime, m_arrFPTransform[FP_LARM]);
}

void CPlayerAnimator::SampleLiminalGunUltimateLoop(float fTime)
{
    SampleKeys(s_arrLiminalGunLoopKeys, s_iLiminalGunLoopKeyCount, fTime, m_arrFPTransform[FP_LARM]);
}

void CPlayerAnimator::SampleLiminalGunUltimateEnd(float fTime)
{
    SampleKeys(s_arrLiminalGunEndKeys, s_iLiminalGunEndKeyCount, fTime, m_arrFPTransform[FP_LARM]);
}

void CPlayerAnimator::ApplyPose()
{
    for (int i = 0; i < TP_END; ++i)
    {
        m_arrBuffer[i].pTransform->Set_Rotation_Raw(m_arrBuffer[i].vInitRot + m_tPose.vRot[i]);
    }
}

TLocoParam CPlayerAnimator::GetLocoParam(EPlayerLocomotionState eLoco)
{
    switch (eLoco)
    {
    case EPlayerLocomotionState::WALK:   return { 7.f,  35.f, 30.f };
    case EPlayerLocomotionState::SPRINT: return { 11.f, 60.f, 55.f };
    case EPlayerLocomotionState::IDLE:
    default:                             return { 7.f,  0.f,  0.f };
    }
}

void CPlayerAnimator::TransformPropagation(const _matrix& matRootWorld)
{
    _matrix matAnimRootWorld = (*m_pRootTransform->Get_World()) * matRootWorld;

	for (int i = 0; i < TP_END; ++i)
	{
		int pp = s_arrUpdateOrder[i];
		int ppParent = s_arrParent[pp];
		if (ppParent == TP_ROOT)
		{
			m_arrBuffer[pp].pTransform->WorldMatrixPropagation(matAnimRootWorld);
		}
		else
		{
			_matrix matParentWorld = *m_arrBuffer[ppParent].pTransform->Get_World();
			m_arrBuffer[pp].pTransform->WorldMatrixPropagation(matParentWorld);
		}
    }
}

void CPlayerAnimator::TransformFPPropagation(const _matrix& matCamWorld)
{
    for (int i = 0; i < FP_END; ++i)
    {
        m_arrFPTransform[i]->WorldMatrixPropagation(matCamWorld);
    }
}

void CPlayerAnimator::SetBuffer(const array<TPlayerBuffer, TP_END>& tBuffer)
{
	m_arrBuffer = tBuffer;

	for (int i = 0; i < TP_END; ++i)
	{
		_vec3 vPivot = m_arrBuffer[i].pBuffer->GetPivot();
		int pp = s_arrParent[i];
		if (pp != TP_ROOT)
		{
			vPivot -= m_arrBuffer[pp].pBuffer->GetPivot();
		}
		m_arrBuffer[i].pBuffer->SetPivot(vPivot);

		m_arrBuffer[i].pTransform->Set_Pos(vPivot);
	}

    SetInitialTransform();
}

void CPlayerAnimator::SetInitialTransform()
{
    for (auto& tPart : m_arrBuffer)
    {
        tPart.vInitPos = tPart.pTransform->Get_Info_Local(INFO_POS);
        tPart.vInitRot = tPart.pTransform->Get_Rotation();
    }
}

void CPlayerAnimator::RenderDebugTransform()
{
    if (!ImGui::Begin("Animation Debug"))
    {
        ImGui::End();
        return;
    }

    ImGui::Checkbox("Pause Animation", &m_bAnimPause);
    ImGui::SameLine();
    if (ImGui::Button("Reset All"))
    {
        for (int i = 0; i < TP_END; ++i)
        {
            m_arrBuffer[i].pTransform->Set_Pos(m_arrBuffer[i].vInitPos);
            m_arrBuffer[i].pTransform->Set_Rotation_Raw(m_arrBuffer[i].vInitRot);
        }
    }

    /* ───────── 3인칭 ───────── */
    if (ImGui::TreeNodeEx("Third Person", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (int i = 0; i < TP_END; ++i)
        {
            ImGui::PushID(i);
            DrawTransformEditor(s_szPartName[i], m_arrBuffer[i].pTransform,
                &m_arrBuffer[i].vInitPos, &m_arrBuffer[i].vInitRot, false);
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    /* ───────── 1인칭 ───────── */
    if (ImGui::TreeNodeEx("First Person", ImGuiTreeNodeFlags_DefaultOpen))
    {
        /* 클립 선택 */
        if (ImGui::BeginCombo("Clip", s_arrFViewDebugClips[m_iDebugClip].szName))
        {
            for (int i = 0; i < s_iFViewDebugClipCount; ++i)
            {
                if (ImGui::Selectable(s_arrFViewDebugClips[i].szName, i == m_iDebugClip))
                {
                    m_iDebugClip = i;
                    m_fDebugScrubTime = 0.f;
                }
            }
            ImGui::EndCombo();
        }

        const TFViewDebugClip& tClip = s_arrFViewDebugClips[m_iDebugClip];

        if (ImGui::Button("Play"))
            PlayAction(tClip.eAction);

        ImGui::SameLine();
        ImGui::Text("t = %.3f", m_fActionTime);

        /* 일시정지 상태에서만 시간 스크럽 */
        if (m_bAnimPause)
        {
            const float fEnd = tClip.pKeys[tClip.iCount - 1].fTime;
            if (ImGui::SliderFloat("Scrub", &m_fDebugScrubTime, 0.f, fEnd, "%.3f s"))
            {
                m_eAction = tClip.eAction;   // 렌더 조건 유지
                m_fActionTime = m_fDebugScrubTime;
                SampleKeys(tClip.pKeys, tClip.iCount, m_fDebugScrubTime, m_arrFPTransform[FP_LARM]);
            }
        }
        else
        {
            ImGui::TextDisabled("Pause to scrub");
        }

        static const char* s_szFPName[FP_END] = { "FP L Arm", "FP R Arm" };
        for (int i = 0; i < FP_END; ++i)
        {
            ImGui::PushID(100 + i);
            DrawTransformEditor(s_szFPName[i], m_arrFPTransform[i], nullptr, nullptr, true);
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    ImGui::End();
}

void CPlayerAnimator::DrawTransformEditor(const char* szName, CTransform* pTransform,
    const _vec3* pInitPos, const _vec3* pInitRot,
    bool bCopyAsKey)
{
    if (nullptr == pTransform)
    {
        ImGui::TextDisabled("%s : (null)", szName);
        return;
    }

    if (!ImGui::CollapsingHeader(szName, ImGuiTreeNodeFlags_DefaultOpen))
        return;

    _vec3 vPos = pTransform->Get_Info_Local(INFO_POS);   // Set_Pos와 같은 공간(로컬)
    _vec3 vRot = pTransform->Get_Rotation();             // degree

    if (ImGui::DragFloat3("Position", &vPos.x, 0.005f, 0.f, 0.f, "%.3f"))
        pTransform->Set_Pos(vPos);

    if (ImGui::DragFloat3("Rotation", &vRot.x, 0.5f, -360.f, 360.f, "%.1f deg"))
        pTransform->Set_Rotation_Raw(vRot);

    if (pInitPos && pInitRot)
    {
        if (ImGui::Button("Reset"))
        {
            pTransform->Set_Pos(*pInitPos);
            pTransform->Set_Rotation_Raw(*pInitRot);
        }
        ImGui::SameLine();
    }

    if (ImGui::Button("Copy"))
    {
        ImGui::LogToClipboard();
        if (bCopyAsKey)   // TArmKey 한 줄 형식 그대로
            ImGui::LogText("{ %.2ff, { %.3ff, %.3ff, %.3ff }, { %.1ff, %.1ff, %.1ff } },",
                m_fActionTime, vPos.x, vPos.y, vPos.z, vRot.x, vRot.y, vRot.z);
        else
            ImGui::LogText("{ %.3ff, %.3ff, %.3ff }, { %.1ff, %.1ff, %.1ff }",
                vPos.x, vPos.y, vPos.z, vRot.x, vRot.y, vRot.z);
        ImGui::LogFinish();
    }
}

CPlayerAnimator* CPlayerAnimator::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayerAnimator* pComponent = new CPlayerAnimator(pGraphicDev);

	return pComponent;
}

void CPlayerAnimator::Free()
{
	CComponent::Free();
}
