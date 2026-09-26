#include "CCameraTPVPerspective.h"
#include "CTransform.h"
#include "CManagement.h"
#include "CDInputMgr.h"


CCameraTPVPerspective::CCameraTPVPerspective(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCameraObj(pGraphicDev)
	, m_fDistance(5.f)
	, m_fHeight(1.f)
{
}

CCameraTPVPerspective::~CCameraTPVPerspective()
{
}

HRESULT CCameraTPVPerspective::Ready_Camera()
{
	m_vEye = { 0.f, 0.f, 0.f };
	m_vAt = { 0.f, 0.f, 0.f };
	m_vUp = { 0.f, 1.f, 0.f };
	m_fFov = D3DXToRadian(60.f);
	m_fAspect = (_float)WINCX / WINCY;
	m_fNear = 0.1f;
	m_fFar = 1000.f;
	m_fAngle = 20.f;

	return S_OK;
}


void CCameraTPVPerspective::Update_Camera(const _float& fTimeDelta, const _vec3& vTargetLook, const _vec3& vTargetPos, const _vec3& vTargetRight)
{
	Mouse_Move();

	_vec3   vLook = vTargetLook;
	_vec3   vPos = vTargetPos;
	_vec3	vRight = vTargetRight;
	_matrix matAxis;

	D3DXMatrixRotationAxis(&matAxis, &vRight, D3DXToRadian(m_fAngle));
	D3DXVec3TransformNormal(&vLook, &vLook, &matAxis);
	D3DXVec3Normalize(&vLook, &vLook);

	// Look at the target's head, and back off along the look direction
	m_vAt = vPos + _vec3(0.f, m_fHeight, 0.f);
	m_vEye = m_vAt - vLook * m_fDistance;

	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
}

void CCameraTPVPerspective::LateUpdate_Camera(const _float& fTimeDelta)
{

}


void CCameraTPVPerspective::Mouse_Move()
{
	_long dwMouseMove(0);
	if (dwMouseMove = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Y))
	{
		m_fAngle += dwMouseMove / 10.f;

		if (m_fAngle > 60.f) m_fAngle = 60.f;
		if (m_fAngle < -10.f) m_fAngle = -10.f;
	}

	if (dwMouseMove = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Z))
	{
		m_fDistance -= dwMouseMove / 120.f * 0.5f;

		if (m_fDistance > 10.f) m_fDistance = 10.f;
		if (m_fDistance < 2.f) m_fDistance = 2.f;
	}
}



CCameraObj* CCameraTPVPerspective::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCameraObj* pCamera = new CCameraTPVPerspective(pGraphicDev);
	if (FAILED(pCamera->Ready_Camera()))
	{
		Safe_Release(pCamera);

		MSG_BOX("Camera Create Failed");
		return nullptr;
	}
	return pCamera;
}

void CCameraTPVPerspective::Free()
{

}
