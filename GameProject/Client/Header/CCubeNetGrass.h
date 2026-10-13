#pragma once

#include "CCubeNet.h"
#include "IVerticalTerrain.h"

class CCubeNetGrass : public CCubeNet, public IVerticalTerrain
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

	/* IVerticalTerrain */
	virtual float SampleTerrainHeight(const _vec3& vRayStart) override;

private:
	HRESULT Add_Component();

public:
	static CCubeNetGrass* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};