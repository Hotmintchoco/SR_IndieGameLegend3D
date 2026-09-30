#pragma once

#include "CGameObject.h"

namespace Engine
{
	struct THitInfo;
	class CVIBuffer;
}

class CRayCaster : public CGameObject
{
protected:
	explicit CRayCaster(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CRayCaster();

public:
	virtual	HRESULT Ready_GameObject();
	virtual	_int Update_GameObject(const _float& fTimeDelta);
	virtual	void LateUpdate_GameObject(const _float& fTimeDelta);
	virtual	void Render_GameObject();

	void RayTest(THitInfo& tHitInfo, const _vec3& vRayStart, const _vec3& vRayDir, CVIBuffer* pBuffer);

private:
	HRESULT	Add_Component();

public:
	static CRayCaster* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;

};

