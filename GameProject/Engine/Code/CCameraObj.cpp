#include "CCameraObj.h"
#include "CCameraFPVPerspective.h"

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
		/*
		case CAMERA_TPV_PERSPECTIVE :
			return CCameraTPVPerspective::Create(pGraphicDev);
		*/
	default:
		return nullptr;
	}
}

void CCameraObj::Get_CamLook(_vec3* pLook)
{
	_vec3 vCamLook = m_vAt - m_vEye;
	D3DXVec3Normalize(pLook, &vCamLook);
}

void CCameraObj::Free()
{
}
