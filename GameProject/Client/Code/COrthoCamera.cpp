#include "pch.h"
#include "COrthoCamera.h"

namespace
{
	template<typename T>
	T Lerp(const T& s, const T& e, const float t)
	{
		return t * e + (1.f - t) * s;
	};

	/* 단위 벡터 가정 */
	_vec3 Slerp(const _vec3& s, const _vec3& e, const float t) 
	{
		float cos = D3DXVec3Dot(&s, &e);
		if (cos > 0.9995f) /* 0 나누기 방지*/
		{
			_vec3 v = Lerp(s, e, t);
			D3DXVec3Normalize(&v, &v);
			return v;
		}
		float th = acosf(cos);
		return (sinf((1.f - t) * th) * s + sinf(t * th) * e) / sinf(th);
	};
}

COrthoCamera::COrthoCamera(LPDIRECT3DDEVICE9 pGraphicDev)
    : CCamera(pGraphicDev)
{
}

COrthoCamera::~COrthoCamera()
{
}

HRESULT COrthoCamera::Ready_GameObject()
{
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
	D3DXMatrixIdentity(&m_matProjOrtho);
	D3DXMatrixIdentity(&m_matProjPersp);

	m_vEye = _vec3{ 0.f, 0.f, 0.f };
	m_vAt = _vec3{ 0.f, 0.f, 0.f };
	m_vUp = _vec3{ 0.f, 1.f, 0.f };

	m_fFov = D3DXToRadian(60.f);
	m_fAspect = static_cast<_float>(WINCX) / WINCY;
	m_fNear = 0.1f;
	m_fFar = 1000.f;

	D3DXMatrixOrthoLH(&m_matProjOrtho, m_fWidth, m_fHeight, m_fNear, m_fFar);
	D3DXMatrixPerspectiveFovLH(&m_matProjPersp, m_fFov, m_fAspect, m_fNear, m_fFar);
	
	if (FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;

    return S_OK;
}

_int COrthoCamera::Update_GameObject(_float fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

    return iExit;
}

void COrthoCamera::LateUpdate_GameObject(_float fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	if (m_bOnViewAnimation)
	{
		m_fTimeAfterViewAnimation += fTimeDelta;

		float r = clamp(m_fTimeAfterViewAnimation / m_fViewAnimationTime, 0.f, 1.f);
		float fStartSize = D3DXVec3Length(&m_vOrbitStartOnSphere);
		float fEndSize = D3DXVec3Length(&m_vOrbitEndOnSphere);
		_vec3 vStartNorm = m_vOrbitStartOnSphere / fStartSize;
		_vec3 vEndNorm = m_vOrbitEndOnSphere / fEndSize;
		_vec3 v = Slerp(vStartNorm, vEndNorm, r);
		float s = Lerp(fStartSize, fEndSize, r);
		_vec3 vEye = m_vOrbitAt + s * v;

		_vec3 vDir = vEye - m_vOrbitAt;

		if (D3DXVec3Dot(&m_vUp, &vDir) > 0.999f)
		{
			_vec3 vUp{ 0.f, 0.f, 1.f };
			D3DXMatrixLookAtLH(&m_matView, &vEye, &m_vOrbitAt, &vUp);
		}
		else
		{
			D3DXMatrixLookAtLH(&m_matView, &vEye, &m_vOrbitAt, &m_vUp);
		}

		/* 끝까지 왔으면 애니메이션 종료 (뷰 행렬은 마지막 값 유지) + 일회용 콜백 */
		if (r >= 1.f)
		{
			m_bOnViewAnimation = false;

			if (m_OrbitFinished)
			{
				/* 콜백 안에서 다시 Orbit을 불러도 되도록 꺼내서 비운 뒤 호출 */
				std::function<void()> F = std::move(m_OrbitFinished);
				m_OrbitFinished = nullptr;
				F();
			}
		}
	}
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	if (m_bOnProjAnimation)
	{
		m_fTimeAfterProjAnimation += fTimeDelta;

		float r = clamp(m_fTimeAfterProjAnimation / m_fProjAnimationTime, 0.f, 1.f);
		const _matrix& m1 = (m_bIsPerspective) ? m_matProjOrtho : m_matProjPersp;
		const _matrix& m2 = (m_bIsPerspective) ? m_matProjPersp : m_matProjOrtho;
		m_matProj = Lerp(m1, m2, r);
		m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

		if (r >= 1.f)
		{
			m_bOnProjAnimation = false;
		}
	}
	else
	{
		const _matrix& m = (m_bIsPerspective) ? m_matProjPersp : m_matProjOrtho;
		m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m);
	}

}

void COrthoCamera::ToggleProjectionMode(const float fDuration)
{
	m_bOnProjAnimation = true;
	m_fProjAnimationTime = fDuration;
	m_fTimeAfterProjAnimation = 0.f;
	m_bIsPerspective = !m_bIsPerspective;
}

void COrthoCamera::UpdateView(const _vec3& vEye, const _vec3& vAt)
{
	// _vec3 vLook, vRight, vUp;
	// 
	// _vec3 vDir = vAt - vEye;
	// D3DXVec3Normalize(&vLook, &vDir);
	// 
	// D3DXVec3Cross(&vRight, &m_vUp, &vLook);
	// D3DXVec3Normalize(&vRight, &vRight);
	// 
	// D3DXVec3Cross(&vUp, &vLook, &vRight);
	// D3DXVec3Normalize(&vUp, &vUp);
	// 
	// _matrix matView;
	// D3DXMatrixIdentity(&matView);
	// memcpy(&matView._11, &vRight, sizeof(_vec3));
	// memcpy(&matView._21, &vUp, sizeof(_vec3));
	// memcpy(&matView._31, &vLook, sizeof(_vec3));
	// memcpy(&matView._41, &vEye, sizeof(_vec3));
	// 
	// m_matView = matView;

	_vec3 vDir = vEye - vAt;

	if (D3DXVec3Dot(&m_vUp, &vDir) > 0.999f)
	{
		_vec3 vUp{ 0.f, 0.f, 1.f };
		D3DXMatrixLookAtLH(&m_matView, &vEye, &vAt, &vUp);
	}
	else
	{
		D3DXMatrixLookAtLH(&m_matView, &vEye, &vAt, &m_vUp);
	}
}

void COrthoCamera::UpdateView(const _matrix& matView)
{
	m_matView = matView;
}

void COrthoCamera::Orbit(const _vec3& vStart, const _vec3& vEnd, const _vec3& vAt, const float fDuration, std::function<void()> onFinished)
{
	m_OrbitFinished = std::move(onFinished);
	m_bOnViewAnimation = true;
	m_fTimeAfterViewAnimation = 0.f;
	m_fViewAnimationTime = fDuration;
	m_vOrbitStartOnSphere = vStart - vAt;
	m_vOrbitEndOnSphere = vEnd - vAt;
	m_vOrbitAt = vAt;
}

COrthoCamera* COrthoCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	COrthoCamera* pCamera = new COrthoCamera(pGraphicDev);

	if (FAILED(pCamera->Ready_GameObject()))
	{
		pCamera->Release();
		return nullptr;
	}

	return pCamera;
}

void COrthoCamera::Free()
{
    CCamera::Free();
}
