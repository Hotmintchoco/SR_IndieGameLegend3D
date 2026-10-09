#pragma once
#include "CItem.h"

class CGem : public CItem
{
protected:
	explicit CGem(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGem(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pSpawner);
	virtual ~CGem();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

protected:
	virtual void Consume() override;

	// Gem Count UI
	void Update_GemCountUI();

public:
	static CGem* Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pSpawner);
	static CGem* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

private:
	virtual void		Free();
};

