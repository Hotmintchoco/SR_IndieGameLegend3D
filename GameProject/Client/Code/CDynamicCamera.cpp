#include "pch.h"
#include "CDynamicCamera.h"
#include "CDInputMgr.h"

CDynamicCamera::CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCamera(pGraphicDev) , m_bFix(true), m_bCheck(true), m_fSpeed(0.f)
{
}

CDynamicCamera::CDynamicCamera(const CDynamicCamera& rhs)
	: CCamera(rhs), m_bFix(rhs.m_bFix), m_bCheck(rhs.m_bCheck), m_fSpeed(rhs.m_fSpeed)
{
}

CDynamicCamera::~CDynamicCamera()
{
}

HRESULT CDynamicCamera::Ready_GameObject(const _vec3* pEye, 
										const _vec3* pAt, 
										const _vec3* pUp,
										const _float& fFov, 
										const _float& fAspect, 
										const _float& fNear, 
										const _float& fFar)
{
	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vUp = *pUp;

	m_fFov = fFov;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	Sync_AnglesFromLook();
	Update_LookFromAngles();

	if (FAILED(CCamera::Ready_GameObject()))
		return E_FAIL;

	m_fSpeed = 10.f;


	return S_OK;
}

_int CDynamicCamera::Update_GameObject(const _float& fTimeDelta)
{
	Key_Input(fTimeDelta);

	if (m_bFix)
	{
		Mouse_Move();
		Mouse_Fix();
	}


	return 0;
}

void CDynamicCamera::LateUpdate_GameObject(const _float& fTimeDelta)
{
	
}

void CDynamicCamera::Key_Input(const _float& fTimeDelta)
{
	_vec3 vLook = m_vAt - m_vEye;
	D3DXVec3Normalize(&vLook, &vLook);
	_vec3 vRight;
	D3DXVec3Cross(&vRight, &m_vUp, &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	if (CDInputMgr::GetInstance()->Key_Press(DIK_RIGHT))
	{
		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * fTimeDelta * m_fSpeed;

		m_vEye += vLength;
		m_vAt  += vLength;
	}

	if (CDInputMgr::GetInstance()->Key_Press(DIK_LEFT))
	{
		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * fTimeDelta * m_fSpeed;

		m_vEye -= vLength;
		m_vAt  -= vLength;
	}

	if (CDInputMgr::GetInstance()->Key_Press(DIK_UP))
	{
		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * fTimeDelta * m_fSpeed;

		m_vEye += vLength;
		m_vAt  += vLength;
	}

	if (CDInputMgr::GetInstance()->Key_Press(DIK_DOWN))
	{
		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * fTimeDelta * m_fSpeed;

		m_vEye -= vLength;
		m_vAt  -= vLength;
	}

	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_TAB))
	{
		if (m_bCheck)
			return;

		m_bCheck = true;

		if (m_bFix)
			m_bFix = false;

		else
			m_bFix = true;

		if (m_bFix)
			while (::ShowCursor(FALSE) >= 0) {}
		else
			while (::ShowCursor(TRUE) < 0) {}

	}
	
	else
	{
		m_bCheck = false;
	}

	
	
	if (false == m_bFix)
		return;

}

void CDynamicCamera::Mouse_Move()
{
    const _long mouseX = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_X);
    const _long mouseY = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Y);
    Rotate(D3DXToRadian(mouseX / 10.f), D3DXToRadian(mouseY / 10.f));
    Update_LookFromAngles();
}
void CDynamicCamera::Mouse_Fix()
{
	POINT			ptMouseCenter{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptMouseCenter);
	SetCursorPos(ptMouseCenter.x, ptMouseCenter.y);
}

CDynamicCamera* CDynamicCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& fFov, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	CDynamicCamera* pCamera = new CDynamicCamera(pGraphicDev);

	if (FAILED(pCamera->Ready_GameObject(pEye, pAt, pUp, fFov, fAspect, fNear, fFar)))
	{
		Safe_Release(pCamera);
		MSG_BOX("camera Create Failed");
		return nullptr;
	}

	return pCamera;
}

void CDynamicCamera::Free()
{
	CCamera::Free();
}
