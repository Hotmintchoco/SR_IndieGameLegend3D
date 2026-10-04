#pragma once

#include "CLiminalObject.h"
#include "ITerrain.h"
#include "IRayTestable.h"

class CLiminalSlope : public CLiminalObject, public ITerrain
{
protected:
	explicit CLiminalSlope(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLiminalSlope();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;

	/* ITerrain */
	virtual float SampleTerrainHeight(const _vec3& vRayStart) override;

protected:
	HRESULT Add_Component();

public:
	static CLiminalSlope* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free();
};

