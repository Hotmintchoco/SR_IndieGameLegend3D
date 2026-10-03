#pragma once

#include "CGameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera : public CGameObject
{
protected:
	explicit CCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);

	// Manager order: Update -> optional LateUpdate -> matrices -> active camera apply.
	void			Update_Matrices();
	void			Apply_Transform();
	void			GetWorld(_matrix* pWorld) const { D3DXMatrixInverse(pWorld, nullptr, &m_matView); }
	_float			Get_Yaw() const { return atan2f(m_vAt.x - m_vEye.x, m_vAt.z - m_vEye.z); }
	_float			Get_Pitch() const { return m_fPitch; }
	HRESULT			Set_PitchLimits(_float fMinPitch, _float fMaxPitch);

protected:
	void			Sync_AnglesFromLook();
	void			Rotate(_float fDeltaYaw, _float fDeltaPitch);
	void			Update_LookFromAngles();

protected:
	_matrix		m_matView, m_matProj;
	_vec3		m_vEye, m_vAt, m_vUp;
	_float		m_fFov, m_fAspect, m_fNear, m_fFar;
	_float		m_fYaw = 0.f;
	_float		m_fPitch = 0.f;
	_float		m_fMinPitch = D3DXToRadian(-89.f);
	_float		m_fMaxPitch = D3DXToRadian(89.f);

protected:
	virtual void Free();
};

END
