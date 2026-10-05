#pragma once

#include "CLiminalObject.h"

class CLiminalCube : public CLiminalObject
{
protected:
	explicit CLiminalCube(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLiminalCube();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;

protected:
	HRESULT Add_Component();

public:
	static CLiminalCube* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free();
};

