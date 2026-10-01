#pragma once

#include "CGameObject.h"
#include "IReflectable.h"
#include "IRayTestable.h"

namespace Engine
{
	class CPlaneTex;
	class CBoxCollider;
	class CTransform;
}

class CFloor : public CGameObject, public IReflectable, public IRayTestable
{
protected:
	explicit CFloor(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFloor();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(const _float& fTimeDelta) override;
	virtual	void LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual	void Render_GameObject() override;

	/* IReflectable */
	virtual const _vec3 GetNormal() override;

	/* IRayTestable */
	virtual vector<pair<Engine::CVIBuffer*, Engine::CTransform*>> GetRayTestTargetInfo() override;

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

