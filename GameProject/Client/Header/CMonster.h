#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CTriCol;
	class CTransform;
	class CCollider;
}

class CMonster : public CGameObject
{
protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonster();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	Engine::CTriCol* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CCollider* m_pColliderCom;

public:
	static CMonster* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

