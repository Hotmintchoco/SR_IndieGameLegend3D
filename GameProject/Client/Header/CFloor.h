#pragma once

#include "CGameObject.h"
#include "IReflectable.h"

namespace Engine
{
	class CBoxCollider;
	class CTransform;
}

class CFloor : public CGameObject, public IReflectable
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

private:
	HRESULT	Add_Component();
	virtual void OnCollisionEnter(CGameObject* pOther) override;

	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CBoxCollider* m_pColliderCom = nullptr;

public:
	static CFloor* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

