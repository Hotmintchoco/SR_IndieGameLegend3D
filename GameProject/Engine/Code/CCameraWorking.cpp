#include "CCameraWorking.h"
#include "CTransform.h"
#include "CManagement.h"
#include "CDInputMgr.h"


CCameraWorking::CCameraWorking(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCameraObj(pGraphicDev), m_bIsMoving(false)
{
}

CCameraWorking::~CCameraWorking()
{
}

HRESULT CCameraWorking::Ready_Camera()
{
	m_vEye = { 0.f, 0.f, 0.f };
	m_vAt = { 0.f, 0.f, 0.f };
	m_vUp = { 0.f, 1.f, 0.f };
	m_fFov = D3DXToRadian(60.f);
	m_fAspect = (_float)WINCX / WINCY;
	m_fNear = 0.1f;
	m_fFar = 1000.f;
	m_fAngle = 0.f;
	m_fMoveTimer = 0.f;

	return S_OK;
}


void CCameraWorking::Update_Camera(const _float& fTimeDelta, const _vec3& vTargetLook, const _vec3& vTargetPos, const _vec3& vTargetRight)
{

}

void CCameraWorking::LateUpdate_Camera(const _float& fTimeDelta)
{
	if (m_queCameraMove.empty()) m_bIsMoving = false;
	if (m_bIsMoving == false) return;

	m_fMoveTimer += fTimeDelta;

	SetEyePos(m_queCameraMove.front());
	SetAtPos(m_queCameraMove.front());

	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);

	if (m_fMoveTimer >= m_queCameraMove.front().fTime)
	{
		m_fMoveTimer = 0.f;
		m_queCameraMove.pop();
	}
}

void CCameraWorking::SetCameraMove(const CAMERA_MOVE& camMoveInfo)
{
	m_queCameraMove.push(camMoveInfo);
	m_bIsMoving = true;
}

void CCameraWorking::ClearCameraMove()
{

	while (!m_queCameraMove.empty())
	{
		m_queCameraMove.pop();
	}
	m_fMoveTimer = 0.f;
	m_bIsMoving = false;
}

HRESULT CCameraWorking::SetEyePos(const CAMERA_MOVE& camMoveInfo)
{
	switch (camMoveInfo.eyeMoveAttr)
	{
	case EYE_STATIC:
	{
		m_vEye = camMoveInfo.vEyeInfo[EYE_POS];
		return S_OK;
	}
	case EYE_LINEAR:
	{
		_vec3 vPos;
		_float fProgress = m_fMoveTimer / camMoveInfo.fTime;
		if (fProgress > 1.f) fProgress = 1.f;
		D3DXVec3Lerp(&vPos, &camMoveInfo.vEyeInfo[EYE_FROM], &camMoveInfo.vEyeInfo[EYE_TO], fProgress);
		m_vEye = vPos;
		return S_OK;
	}
	case EYE_TRACE:
	{
		CTransform* pTransform = nullptr;
		pTransform = dynamic_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, camMoveInfo.pEyeTraceTarget.first, camMoveInfo.pEyeTraceTarget.second, L"Com_Transform"));
		if (pTransform == nullptr) return E_FAIL;
		_vec3 vRelative = camMoveInfo.vEyeInfo[EYE_TRACE_RELATIVE];
		_vec3 vScale = pTransform->m_vScale;
		_vec3 vLocal(vRelative.x / vScale.x, vRelative.y / vScale.y, vRelative.z / vScale.z);

		D3DXVec3TransformCoord(&m_vEye, &vLocal, pTransform->Get_World());

		return S_OK;
	}
	default:
	{
		return E_FAIL;
	}
	}
}

HRESULT CCameraWorking::SetAtPos(const CAMERA_MOVE& camMoveInfo)
{
	switch (camMoveInfo.atMoveAttr)
	{
	case AT_STATIC : 
	{
		m_vAt = m_vEye + camMoveInfo.vAtInfo[AT_DIRECTION];
		return S_OK;
	}
	case AT_POINT:
	{
		m_vAt = camMoveInfo.vAtInfo[AT_POS];
		return S_OK;
	}
	case AT_POINT_LINEAR:
	{
		_vec3 vPos;
		_float fProgress = m_fMoveTimer / camMoveInfo.fTime;
		if (fProgress > 1.f) fProgress = 1.f;
		D3DXVec3Lerp(&vPos, &camMoveInfo.vAtInfo[AT_FROM], &camMoveInfo.vAtInfo[AT_TO], fProgress);
		m_vAt = vPos;
		return S_OK;
	}
	case AT_TRACE:
	{
		CTransform* pTransform = nullptr;
		pTransform = dynamic_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, camMoveInfo.pAtTraceTarget.first, camMoveInfo.pAtTraceTarget.second, L"Com_Transform"));
		if (pTransform == nullptr) return E_FAIL;
		_vec3 vRelative = camMoveInfo.vAtInfo[AT_TRACE_RELATIVE];
		_vec3 vScale = pTransform->m_vScale;
		_vec3 vLocal(vRelative.x / vScale.x, vRelative.y / vScale.y, vRelative.z / vScale.z);

		D3DXVec3TransformCoord(&m_vAt, &vLocal, pTransform->Get_World());

		return S_OK;
	}
	default:
	{
		return E_FAIL;
	}
	}
}

CCameraObj* CCameraWorking::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCameraObj* pCamera = new CCameraWorking(pGraphicDev);
	if (FAILED(pCamera->Ready_Camera()))
	{
		Safe_Release(pCamera);

		MSG_BOX("Camera Create Failed");
		return nullptr;
	}
	return pCamera;
}

void CCameraWorking::Free()
{

}
