#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CBoxCollider;
	class CTransform;
}

class CTriggerBox : public CGameObject
{
protected:
	explicit CTriggerBox(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTriggerBox();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

	virtual void OnCollisionEnter(CGameObject* pOther) override;

private:
	Engine::CTransform* m_pTransformCom;
	Engine::CBoxCollider* m_pColliderCom;

public:
	static CTriggerBox* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

