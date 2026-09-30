#include "CCameraObj.h"
#include "CCameraFPVPerspective.h"
#include "CCameraTPVPerspective.h"
#include "CCameraFreePerspective.h"
#include "CCameraWorking.h"

CCameraObj::CCameraObj(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
}

CCameraObj::~CCameraObj()
{
}


CCameraObj* CCameraObj::Create(CAMERAID tagCameraType, LPDIRECT3DDEVICE9 pGraphicDev)
{
	switch (tagCameraType)
	{
	case CAMERA_FPV_PERSPECTIVE:
		return CCameraFPVPerspective::Create(pGraphicDev);
	case CAMERA_TPV_PERSPECTIVE:
		return CCameraTPVPerspective::Create(pGraphicDev);
	case CAMERA_FREE_PERSPECTIVE:
		return CCameraFreePerspective::Create(pGraphicDev);
	case CAMERA_WORKING:
		return CCameraWorking::Create(pGraphicDev);
	default:
		return nullptr;
	}
}

void CCameraObj::Apply_Transform()
{
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
}

void CCameraObj::Set_View(const _vec3& vEye, const _vec3& vAt)
{
	m_vEye = vEye;
	m_vAt = vAt;

	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
}

void CCameraObj::Get_CamLook(_vec3* pLook)
{
	_vec3 vCamLook = m_vAt - m_vEye;
	D3DXVec3Normalize(pLook, &vCamLook);
}

void CCameraObj::Free()
{
}
