#include "CCamera.h"

CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_fAspect(0.f), m_fFov(0.f), m_fNear(0.f), m_fFar(0.f)
	, m_vEye({ 0.f, 0.f, 0.f })
	, m_vAt({ 0.f, 0.f, 0.f })
	, m_vUp({ 0.f, 0.f, 0.f })
{
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
}

CCamera::CCamera(const CCamera& rhs)
	: CGameObject(rhs)
	, m_fAspect(rhs.m_fAspect)
	, m_fFov(rhs.m_fFov)
	, m_fNear(rhs.m_fNear)
	, m_fFar(rhs.m_fFar)
	, m_vEye(rhs.m_vEye)
	, m_vAt(rhs.m_vAt)
	, m_vUp(rhs.m_vUp)
	, m_fYaw(rhs.m_fYaw), m_fPitch(rhs.m_fPitch)
	, m_fMinPitch(rhs.m_fMinPitch), m_fMaxPitch(rhs.m_fMaxPitch)
{
	m_matView = rhs.m_matView;
	m_matProj = rhs.m_matProj;
}

CCamera::~CCamera()
{
}

HRESULT CCamera::Ready_GameObject()
{
	Update_Matrices();
	return S_OK;
}

_int CCamera::Update_GameObject(_float fTimeDelta)
{
	return 0;
}

HRESULT CCamera::Set_PitchLimits(_float fMinPitch, _float fMaxPitch)
{
	const _float fSafeLimit = D3DXToRadian(89.9f);
	if (!std::isfinite(fMinPitch) || !std::isfinite(fMaxPitch) ||
		fMinPitch > fMaxPitch || fMinPitch < -fSafeLimit || fMaxPitch > fSafeLimit)
		return E_INVALIDARG;
	m_fMinPitch = fMinPitch;
	m_fMaxPitch = fMaxPitch;
	Sync_AnglesFromLook();
	Update_LookFromAngles();
	return S_OK;
}

void CCamera::Sync_AnglesFromLook()
{
	const _vec3 vLook = m_vAt - m_vEye;
	const _float fHorizontal = sqrtf(vLook.x * vLook.x + vLook.z * vLook.z);
	if (D3DXVec3LengthSq(&vLook) < 1.e-8f)
	{
		m_fYaw = 0.f;
		m_fPitch = 0.f;
	}
	else
	{
		if (fHorizontal > 1.e-6f) m_fYaw = atan2f(vLook.x, vLook.z);
		m_fPitch = atan2f(-vLook.y, fHorizontal);
	}
	m_fPitch = max(m_fMinPitch, min(m_fPitch, m_fMaxPitch));
}

void CCamera::Rotate(_float fDeltaYaw, _float fDeltaPitch)
{
	if (!std::isfinite(fDeltaYaw) || !std::isfinite(fDeltaPitch)) return;
	m_fYaw = fmodf(m_fYaw + fDeltaYaw, 2.f * D3DX_PI);
	m_fPitch = max(m_fMinPitch, min(m_fPitch + fDeltaPitch, m_fMaxPitch));
}

void CCamera::Update_LookFromAngles()
{
	const _float fCosPitch = cosf(m_fPitch);
	const _vec3 vLook{ sinf(m_fYaw) * fCosPitch, -sinf(m_fPitch), cosf(m_fYaw) * fCosPitch };
	m_vAt = m_vEye + vLook;
	m_vUp = { 0.f, 1.f, 0.f };
}

void CCamera::Update_Matrices()
{
	const _vec3 vRenderEye = m_vEye + m_vViewOffset;
	const _vec3 vRenderAt = m_vAt + m_vViewOffset;

	D3DXMatrixLookAtLH(&m_matView, &vRenderEye, &vRenderAt, &m_vUp);
	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
}

void CCamera::Apply_Transform()
{
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
}

void CCamera::LateUpdate_GameObject(_float fTimeDelta)
{
}

void CCamera::Free()
{
	CGameObject::Free();
}
