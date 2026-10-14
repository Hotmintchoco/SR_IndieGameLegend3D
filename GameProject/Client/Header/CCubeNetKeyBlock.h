#pragma once

#include "CCubeNet.h"
#include "IVerticalTerrain.h"

/* 열쇠 블럭 : 페이즈의 키를 모두 모으면 방 레이어가 파괴한다 */
class CCubeNetKeyBlock : public CCubeNet, public IVerticalTerrain
{
protected:
	explicit CCubeNetKeyBlock(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCubeNetKeyBlock();

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
	static CCubeNetKeyBlock* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};
