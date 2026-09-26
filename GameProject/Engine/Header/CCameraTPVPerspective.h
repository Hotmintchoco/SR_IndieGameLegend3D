#pragma once
#include "CCameraObj.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CCameraTPVPerspective : public CCameraObj
{
private:
	explicit CCameraTPVPerspective(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCameraTPVPerspective();

public:
	HRESULT			Ready_Camera() override;
	void			Update_Camera(const _float& fTimeDelta, const _vec3& vTargetLook, const _vec3& vTargetPos, const _vec3& vTargetRight) override;
	void			LateUpdate_Camera(const _float& fTimeDelta) override;

private:
	void			Mouse_Move();

private:
	_float			m_fDistance;
	_float			m_fHeight;

public:
	static CCameraObj* Create(LPDIRECT3DDEVICE9 pGraphicDev);


private:
	virtual void	Free();

};

END
