#pragma once

#include "CTrail.h"
#include "CProjectile.h"

namespace Engine
{
	class CRcColCustom;
}

class CBullet_Trail : public CTrail
{
protected:
	explicit CBullet_Trail(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBullet_Trail();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	HRESULT			Add_Component();

	void Set_Bullet(CProjectile* pBullet) { m_pBullet = pBullet; }

public:
	static CBullet_Trail* Create(LPDIRECT3DDEVICE9 pGraphicDev, CProjectile* pBullet);

private:
	CProjectile* m_pBullet = nullptr;


protected:
	virtual void		Free();
};

