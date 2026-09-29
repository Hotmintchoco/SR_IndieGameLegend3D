#include "CCameraFreePerspective.h"
#include "CTransform.h"
#include "CManagement.h"
#include "CDInputMgr.h"


CCameraFreePerspective::CCameraFreePerspective(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCameraObj(pGraphicDev)
	, m_fDistance(2.5f), m_vPos({ 60.f, 1.f, 60.f }), m_vDir({ 1.f, 0.f, 0.f }), m_vRight({0.f, 0.f, 1.f})
{
}

CCameraFreePerspective::~CCameraFreePerspective()
{
}

HRESULT CCameraFreePerspective::Ready_Camera()
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


void CCameraFreePerspective::Update_Camera(const _float& fTimeDelta, const _vec3& vTargetLook, const _vec3& vTargetPos, const _vec3& vTargetRight)
{

	_vec3   vLook = m_vDir;
	_vec3	vRight = m_vRight;
	_matrix matAxis;

	D3DXMatrixRotationAxis(&matAxis, &vRight, D3DXToRadian(m_fAngle));
	D3DXVec3TransformNormal(&vLook, &vLook, &matAxis);
	D3DXVec3Normalize(&vLook, &vLook);
	
	Key_Input(fTimeDelta, vLook, vRight);

	m_vAt = m_vPos;
	m_vEye = m_vAt - vLook * m_fDistance;

	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
}

void CCameraFreePerspective::LateUpdate_Camera(const _float& fTimeDelta)
{

}


void CCameraFreePerspective::Mouse_Move()
{
	_long dwMouseMove(0);

	if (dwMouseMove = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_X))
	{
		_matrix		matRot;
		D3DXMatrixRotationY(&matRot, D3DXToRadian(dwMouseMove / 10.f));
		D3DXVec3TransformNormal(&m_vDir, &m_vDir, &matRot);
		D3DXVec3TransformNormal(&m_vRight, &m_vRight, &matRot);
	}

	if (dwMouseMove = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Y))
	{
		m_fAngle -= dwMouseMove / 10.f;

		if (m_fAngle > 80.f) m_fAngle = 80.f;
		if (m_fAngle < -80.f) m_fAngle = -80.f;
	}

	if (dwMouseMove = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Z))
	{
		m_fDistance -= dwMouseMove / 120.f * 0.5f;

		if (m_fDistance > 5.f) m_fDistance = 5.f;
		if (m_fDistance < 1.f) m_fDistance = 1.f;
	}
}

void CCameraFreePerspective::Reset_Setting()
{
	m_fAngle = 0.f;
	CTransform* pPlayerTrans = static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
	_vec3 vPlayerPos = { 60.f, 1.f, 60.f };
	if (pPlayerTrans != nullptr) pPlayerTrans->Get_Info(INFO_POS, &vPlayerPos);
	m_vPos = vPlayerPos;
}



CCameraObj* CCameraFreePerspective::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCameraObj* pCamera = new CCameraFreePerspective(pGraphicDev);
	if (FAILED(pCamera->Ready_Camera()))
	{
		Safe_Release(pCamera);

		MSG_BOX("Camera Create Failed");
		return nullptr;
	}
	return pCamera;
}

void CCameraFreePerspective::Free()
{

}

void CCameraFreePerspective::Key_Input(const _float& fTimeDelta, _vec3 vDir, _vec3 vRight)
{
	_float fSpeed = 10.f;

	if (CDInputMgr::GetInstance()->Key_Press(DIK_W))
	{
		m_vPos += vDir * fSpeed * fTimeDelta;
	}

	if (CDInputMgr::GetInstance()->Key_Press(DIK_S))
	{
		m_vPos += vDir * -fSpeed * fTimeDelta;
	}

	if (CDInputMgr::GetInstance()->Key_Press(DIK_A))
	{
		m_vPos += vRight * -fSpeed * fTimeDelta;
	}

	if (CDInputMgr::GetInstance()->Key_Press(DIK_D))
	{
		m_vPos += vRight * fSpeed * fTimeDelta;
	}

	if (CDInputMgr::GetInstance()->Key_Press(DIK_LSHIFT))
	{
		_vec3 vUp = { 0.f, 1.f, 0.f };
		m_vPos -= vUp * fSpeed * fTimeDelta;
	}

	if (CDInputMgr::GetInstance()->Key_Press(DIK_SPACE))
	{
		_vec3 vUp = { 0.f, 1.f, 0.f };
		m_vPos += vUp * fSpeed * fTimeDelta;
	}
}