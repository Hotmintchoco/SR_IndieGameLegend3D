#pragma once

#include "CGameObject.h"

class CCameraHelper : public CGameObject
{
protected:
	explicit CCameraHelper(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCameraHelper();

public:
	virtual	HRESULT Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	// void DollyZoom(bool bIn, float fDuration);
	// void RoomCamera();

private:
	HRESULT	Add_Component();

public:
	static CCameraHelper* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;

};