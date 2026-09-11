#include "CCameraOld.h"
#include "CPipeline.h"

CCameraOld::CCameraOld(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_fAspect(0.f), m_fFov(0.f), m_fNear(0.f), m_fFar(0.f)
	, m_vEye({ 0.f, 0.f, 0.f })
	, m_vAt({ 0.f, 0.f, 0.f })
	, m_vUp({ 0.f, 0.f, 0.f })
{
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
}

CCameraOld::CCameraOld(const CCameraOld& rhs)
	: CGameObject(rhs)
	, m_fAspect(rhs.m_fAspect)
	, m_fFov(rhs.m_fFov)
	, m_fNear(rhs.m_fNear)
	, m_fFar(rhs.m_fFar)
	, m_vEye(rhs.m_vEye)
	, m_vAt(rhs.m_vAt)
	, m_vUp(rhs.m_vUp)
{
	m_matView = rhs.m_matView;
	m_matProj = rhs.m_matProj;
}

CCameraOld::~CCameraOld()
{
}

HRESULT CCameraOld::Ready_GameObject()
{
	// CPipeline::MakeLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);


	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
	//CPipeline::MakeProjMatrix(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	return S_OK;
}

_int CCameraOld::Update_GameObject(const _float& fTimeDelta)
{
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	//CPipeline::MakeLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	return 0;
}

void CCameraOld::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CCameraOld::Free()
{
	CGameObject::Free();
}
