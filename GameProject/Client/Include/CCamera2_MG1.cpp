#include "pch.h"
#include "CCamera2_MG1.h"
#include "CDInputMgr.h"
#include "CTransform.h"

CCamera2_MG1::CCamera2_MG1(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCamera(pGraphicDev), m_bFix(true), m_bCheck(true), m_fSpeed(0.f)
{
}

CCamera2_MG1::CCamera2_MG1(const CCamera2_MG1& rhs)
	: CCamera(rhs), m_bFix(rhs.m_bFix), m_bCheck(rhs.m_bCheck), m_fSpeed(rhs.m_fSpeed)
{
}

CCamera2_MG1::~CCamera2_MG1()
{
}

HRESULT CCamera2_MG1::Ready_GameObject(const _vec3* pEye,
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

_int CCamera2_MG1::Update_GameObject(_float fTimeDelta)
{
	Key_Input(fTimeDelta);


	Mouse_Move();
	//Mouse_Fix();



	return 0;
}

void CCamera2_MG1::LateUpdate_GameObject(_float fTimeDelta)
{
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
}

void CCamera2_MG1::Set_CameraActive(CTransform* pTransform)
{
	_vec3 vPos; pTransform->Get_Info(INFO_POS, &vPos);
	_vec3 vLook; pTransform->Get_Info(INFO_LOOK, &vLook);
	_vec3 vUp; pTransform->Get_Info(INFO_UP, &vUp);
	D3DXVec3Normalize(&vLook, &vLook);

	m_vEye = vPos - vLook * 3.f + vUp * 2.f;
	m_vAt = vPos;
	m_vUp = { 0.f,1.f,0.f };

	Sync_AnglesFromLook();
	//Update_LookFromAngles();
}

void CCamera2_MG1::Key_Input(const _float fTimeDelta)
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
		m_vAt += vLength;
	}

	if (CDInputMgr::GetInstance()->Key_Press(DIK_LEFT))
	{
		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * fTimeDelta * m_fSpeed;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (CDInputMgr::GetInstance()->Key_Press(DIK_UP))
	{
		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * fTimeDelta * m_fSpeed;

		m_vEye += vLength;
		m_vAt += vLength;
	}

	if (CDInputMgr::GetInstance()->Key_Press(DIK_DOWN))
	{
		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * fTimeDelta * m_fSpeed;

		m_vEye -= vLength;
		m_vAt -= vLength;
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

void CCamera2_MG1::Mouse_Move()
{
	const _long mouseX = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_X);
	const _long mouseY = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Y);
	Rotate(D3DXToRadian(mouseX / 10.f), D3DXToRadian(mouseY / 10.f));
	Update_LookFromAngles();
}

void CCamera2_MG1::Mouse_Fix()
{
	POINT			ptMouseCenter{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptMouseCenter);
	SetCursorPos(ptMouseCenter.x, ptMouseCenter.y);
}

CCamera2_MG1* CCamera2_MG1::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& fFov, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	CCamera2_MG1* pCamera = new CCamera2_MG1(pGraphicDev);

	if (FAILED(pCamera->Ready_GameObject(pEye, pAt, pUp, fFov, fAspect, fNear, fFar)))
	{
		Safe_Release(pCamera);
		MSG_BOX("camera Create Failed");
		return nullptr;
	}

	return pCamera;
}

void CCamera2_MG1::Free()
{
	CCamera::Free();
}
