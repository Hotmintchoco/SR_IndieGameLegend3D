#pragma once

#include "CGameObject.h"
#include "IRayTestable.h"
#include "ITerrain.h"

namespace Engine
{
	class CPlaneTex;
	class CBoxCollider;
	class CTransform;
}

class CFloor : public CGameObject, public IRayTestable, public ITerrain
{
protected:
	explicit CFloor(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFloor();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	/* IRayTestable */
	virtual vector<pair<Engine::CVIBuffer*, Engine::CTransform*>> GetRayTestTargetInfo() override;

	/* ITerrain */
	virtual float SampleTerrainHeight(const _vec3& vRayStart) override;

private:
	HRESULT	Add_Component();
	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;

	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CBoxCollider* m_pColliderCom = nullptr;
	Engine::CPlaneTex* m_pBufferCom = nullptr;

public:
	static CFloor* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

