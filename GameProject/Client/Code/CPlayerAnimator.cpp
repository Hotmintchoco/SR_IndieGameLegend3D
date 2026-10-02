#include "pch.h"
#include "CPlayerAnimator.h"
#include "CTransform.h"
#include "CPlayerPartTex.h"


// enum PLAYERPART { PP_BODY, PP_HEAD, PP_LARM, PP_RARM, PP_LLEG, PP_RLEG, PP_END };
const array<int, PP_END> CPlayerAnimator::s_arrParent =
{ PP_ROOT, PP_BODY, PP_BODY, PP_BODY, PP_ROOT, PP_ROOT };

const array<PLAYERPART, PP_END> CPlayerAnimator::s_arrUpdateOrder =
{ PP_BODY, PP_LLEG, PP_RLEG, PP_HEAD, PP_LARM, PP_RARM };

CPlayerAnimator::CPlayerAnimator(LPDIRECT3DDEVICE9 pGraphicDev)
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
		m_arrBuffer[i].pTransform->UpdateLocalMatrix();
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
