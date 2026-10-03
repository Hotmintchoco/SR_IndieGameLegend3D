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
	return S_OK;
}

void CPlayerAnimator::LateUpdate_Component()
{
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
