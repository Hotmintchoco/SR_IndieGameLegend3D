#pragma once
#include "CCamera.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CCameraFPVPerspective : public CCamera
{
private:
	explicit CCameraFPVPerspective(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCameraFPVPerspective();

public:
	HRESULT			Ready_Camera() override;
	void			Update_Camera(const _float& fTimeDelta) override;
	void			LateUpdate_Camera(const _float& fTimeDelta) override;

private : 
	void			Mouse_Move();

public:
	static CCamera*	Create(LPDIRECT3DDEVICE9 pGraphicDev);


private:
	virtual void	Free();

};

END