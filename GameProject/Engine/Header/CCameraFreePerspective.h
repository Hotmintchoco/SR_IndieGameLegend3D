#pragma once
#include "CCameraObj.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CCameraFreePerspective : public CCameraObj
{
private:
	explicit CCameraFreePerspective(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCameraFreePerspective();

public:
	HRESULT			Ready_Camera() override;
	void			Update_Camera(const _float& fTimeDelta, const _vec3& vTargetLook, const _vec3& vTargetPos, const _vec3& vTargetRight) override;
	void			LateUpdate_Camera(const _float& fTimeDelta) override;
	void			Input_Camera() override { Mouse_Move(); }
	void			Reset_Setting() override { m_fAngle = 0.f; m_vPos = { 60.f, 1.f, 60.f }; m_vDir = { 1.f, 0.f, 0.f }; m_vRight={ 0.f, 0.f, 1.f }; }

private:
	void			Mouse_Move();
	void			Key_Input(const _float& fTimeDelta, _vec3 vDir, _vec3 vRight);
	_vec3			m_vPos, m_vDir, m_vRight;

private:
	_float			m_fDistance;

public:
	static CCameraObj* Create(LPDIRECT3DDEVICE9 pGraphicDev);


private:
	virtual void	Free();

};

END
