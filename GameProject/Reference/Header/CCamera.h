#pragma once

#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CCamera : public CBase
{
protected:
	explicit CCamera(LPDIRECT3DDEVICE9 pGraphicDev) ;
	virtual ~CCamera();


public:
	virtual	HRESULT		Ready_Camera() = 0;
	virtual void		Update_Camera(const _float& fTimeDelta) = 0;
	virtual void		LateUpdate_Camera(const _float& fTimeDelta) = 0;
	void				Get_CamLook(_vec3* pLook);
	void				Get_CameraAngle(_float* pAngle) { memcpy( pAngle, &m_fAngle, sizeof(_float)); }

protected:
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	_matrix				m_matView, m_matProj;
	_vec3				m_vEye, m_vAt, m_vUp;
	_float				m_fFov, m_fAspect, m_fNear, m_fFar;
	_float				m_fAngle;

public:
	static CCamera* Create(CAMERAID tagCameraType, LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void	Free();
};

END