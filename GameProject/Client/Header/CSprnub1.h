#pragma once

#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
}

class CSprnub1 : public CMonster
{
protected:
	explicit CSprnub1(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSprnub1();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(COLLINFO eCollInfo) override;

private:
	HRESULT			Add_Component();

public:
	static CSprnub1* Create(LPDIRECT3DDEVICE9 pGraphicDev);


private:


protected:
	virtual void		Free();
};
