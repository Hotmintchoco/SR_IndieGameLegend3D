#include "pch.h"
#include "CLiminalBomb.h"
#include "CProtoMgr.h"

_int CLiminalBomb::Update_GameObject(_float fTimeDelta)
{
	m_pTransformCom->Move_Pos(&m_vDir, m_fSpeed, fTimeDelta);

	return S_OK;
}

float CLiminalBomb::GetScale()
{
	return m_pTransformCom->Get_Scale().x;
}

void CLiminalBomb::SetScale(float fScale)
{
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);
}
