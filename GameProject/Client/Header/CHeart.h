#pragma once
#include "CItem.h"

class CHeart : public CItem
{
protected:
	explicit CHeart(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CHeart(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pSpawner);
	virtual ~CHeart();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

protected:
	virtual void Consume() override;

public:
	static CHeart* Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pSpawner);

private:
	virtual void		Free();
};

