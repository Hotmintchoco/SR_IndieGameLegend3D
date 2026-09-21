#include "CCameraFPVPerspective.h"
#include "CTransform.h"
#include "CManagement.h"
#include "CDInputMgr.h"


CCameraFPVPerspective::CCameraFPVPerspective(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCameraObj(pGraphicDev)
{
}

CCameraFPVPerspective::~CCameraFPVPerspective()
{
}

HRESULT CCameraFPVPerspective::Ready_Camera()
{
	m_vEye = { 0.f, 0.f, 0.f };
	m_vAt = { 0.f, 0.f, 0.f };
	m_vUp = { 0.f, 1.f, 0.f };
	m_fFov = D3DXToRadian(60.f);
	m_fAspect = (_float)WINCX / WINCY;
	m_fNear = 0.1f;
	m_fFar = 1000.f;
	m_fAngle = 0.f;

	return S_OK;
}


void CCameraFPVPerspective::Update_Camera(const _float& fTimeDelta, const _vec3& vTargetLook, const _vec3& vTargetPos, const _vec3& vTargetRight)
{
	Mouse_Move();

	_vec3   vLook = vTargetLook;
	_vec3   vPos = vTargetPos;
	_vec3	vRight = vTargetRight;
	_matrix matAxis;

	D3DXMatrixRotationAxis(&matAxis, &vRight, D3DXToRadian(m_fAngle));
	D3DXVec3TransformNormal(&vLook, &vLook, &matAxis);

	m_vEye = vPos - [&]()->_vec3 {_vec3 vLookDst;  D3DXVec3Normalize(&vLookDst, &vLook); return vLookDst * 0.5f; }();
	m_vAt = vPos + [&]()->_vec3 {_vec3 vLookDst;  D3DXVec3Normalize(&vLookDst, &vLook); return vLookDst * 5.0f; }();

	//m_vEye = vPos;
	//m_vAt = vPos + vLook;

	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
}

void CCameraFPVPerspective::LateUpdate_Camera(const _float& fTimeDelta)
{

}


void CCameraFPVPerspective::Mouse_Move()
{
	_long dwMouseMove(0);
	if (dwMouseMove = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Y))
	{
		m_fAngle += dwMouseMove / 10.f;

		if (m_fAngle > 50.f) m_fAngle = 50.f;
		if (m_fAngle < -80.f) m_fAngle = -80.f;
	}
}



CCameraObj* CCameraFPVPerspective::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCameraObj* pCamera = new CCameraFPVPerspective(pGraphicDev);
	if (FAILED(pCamera->Ready_Camera()))
	{
		Safe_Release(pCamera);

		MSG_BOX("Camera Create Failed");
		return nullptr;
	}
	return pCamera;
}

void CCameraFPVPerspective::Free()
{

}