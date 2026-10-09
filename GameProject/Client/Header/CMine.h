#pragma once

#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
}

class CMine : public CMonster
{
protected:
	explicit CMine(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMine();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(COLLINFO eCollInfo) override;

	void Check_Hp(_float& fTimeDelta) override;
	void Animation_Monster(const _float& fTimeDelta) override;

private:
	HRESULT			Add_Component();

public:
	static CMine* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};
