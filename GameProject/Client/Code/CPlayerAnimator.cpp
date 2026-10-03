#include "pch.h"
#include "CPlayerAnimator.h"
#include "CTransform.h"
#include "CPlayerPartTex.h"
#include "CImGUITool.h"


// enum PLAYERPART { PP_BODY, PP_HEAD, PP_LARM, PP_RARM, PP_LLEG, PP_RLEG, PP_END };
const array<int, PP_END> CPlayerAnimator::s_arrParent =
{ PP_ROOT, PP_BODY, PP_BODY, PP_BODY, PP_ROOT, PP_ROOT };

const array<PLAYERPART, PP_END> CPlayerAnimator::s_arrUpdateOrder =
{ PP_BODY, PP_LLEG, PP_RLEG, PP_HEAD, PP_LARM, PP_RARM };

const char* CPlayerAnimator::s_szPartName[PP_END] =
{ "Body", "Head", "L Arm", "R Arm", "L Leg", "R Leg" };

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

    m_tPose.vRot[PP_LLEG].x += fLeg;
    m_tPose.vRot[PP_RLEG].x -= fLeg;
    m_tPose.vRot[PP_LARM].x -= fArm;
    m_tPose.vRot[PP_RARM].x += fArm;
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
    for (int i = 0; i < PP_END; ++i)
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
    m_tActionPose.bMask[PP_RARM] = true;
    m_tActionPose.vRot[PP_RARM].x = s_fForward * (90.f + s_tShootParam.fKick * fRecoil);   // 정면 90도 + 위로 반동
}

void CPlayerAnimator::ApplyPose()
{
    for (int i = 0; i < PP_END; ++i)
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
	for (int i = 0; i < PP_END; ++i)
	{
		int pp = s_arrUpdateOrder[i];
		int ppParent = s_arrParent[pp];
		if (ppParent == PP_ROOT)
		{
			m_arrBuffer[pp].pTransform->WorldMatrixPropagation(matRootWorld);
		}
		else
		{
			_matrix matParentWorld = *m_arrBuffer[ppParent].pTransform->Get_World();
			m_arrBuffer[pp].pTransform->WorldMatrixPropagation(matParentWorld);
		}
    }
}

void CPlayerAnimator::SetBuffer(const array<TPlayerBuffer, PP_END>& tBuffer)
{
	m_arrBuffer = tBuffer;

	for (int i = 0; i < PP_END; ++i)
	{
		_vec3 vPivot = m_arrBuffer[i].pBuffer->GetPivot();
		int pp = s_arrParent[i];
		if (pp != PP_ROOT)
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
        for (int i = 0; i < PP_END; ++i)
        {
            m_arrBuffer[i].pTransform->Set_Pos(m_arrBuffer[i].vInitPos);
            m_arrBuffer[i].pTransform->Set_Rotation_Raw(m_arrBuffer[i].vInitRot);
        }
    }
    ImGui::Separator();

    for (int i = 0; i < PP_END; ++i)
    {
        CTransform* pTransform = m_arrBuffer[i].pTransform;
        if (nullptr == pTransform)
            continue;

        ImGui::PushID(i);   // 부위마다 같은 라벨("Position" 등)을 쓰므로 ID 분리 필수

        /* mini header : Part Name */
        if (ImGui::CollapsingHeader(s_szPartName[i], ImGuiTreeNodeFlags_DefaultOpen))
        {
            /* Getter */
            _vec3 vPos{}, vRot{};
            pTransform->Get_Info(INFO_POS, &vPos);
            vRot = pTransform->Get_Rotation();          // degree

            /* position (each part local) */
            if (ImGui::DragFloat3("Position", &vPos.x, 0.01f, 0.f, 0.f, "%.3f"))
                pTransform->Set_Pos(vPos);

            /* rotation (each part local) : degree 그대로 사용 */
            if (ImGui::DragFloat3("Rotation", &vRot.x, 0.5f, -360.f, 360.f, "%.1f deg"))
                pTransform->Set_Rotation_Raw(vRot);     // degree

            if (ImGui::Button("Reset"))
            {
                pTransform->Set_Pos(m_arrBuffer[i].vInitPos);
                pTransform->Set_Rotation_Raw(m_arrBuffer[i].vInitRot);
            }
            ImGui::SameLine();
            if (ImGui::Button("Copy"))   // 키프레임 값으로 옮겨 적기 편하게
            {
                ImGui::LogToClipboard();
                ImGui::LogText("{ %.3ff, %.3ff, %.3ff }, { %.1ff, %.1ff, %.1ff }",
                    vPos.x, vPos.y, vPos.z, vRot.x, vRot.y, vRot.z);
                ImGui::LogFinish();
            }
        }

        ImGui::PopID();
    }

    ImGui::End();
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
