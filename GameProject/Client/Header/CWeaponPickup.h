#pragma once

#include "CItem.h"
#include "Client_Enum.h"

class CWeaponPickup : public CItem
{
protected:
	explicit CWeaponPickup(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pSpawner, EObjectType eWeaponType);
	virtual ~CWeaponPickup();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

	EObjectType m_eWeaponType = EObjectType::WEAPON_NONE;

protected:
	virtual void Consume() override;

public:
	static CWeaponPickup* Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pSpawner, EObjectType eWeaponType);

private:
	virtual void		Free();
};