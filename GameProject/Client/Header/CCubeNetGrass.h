#pragma once

#include "CCubeNet.h"

class CCubeNetGrass : public CCubeNet
{
protected:
	explicit CCubeNetGrass(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCubeNetGrass();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	virtual void OnCollisionStay(COLLINFO eCollInfo) override;

private:
	HRESULT Add_Component();

public:
	static CCubeNetGrass* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};