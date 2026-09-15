#pragma once

#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
}

class CBoss1 : public CMonster
{
protected:
	explicit CBoss1(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBoss1();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(CGameObject* pOther) override;

private:
	HRESULT			Add_Component();

protected:
	Engine::CTexture* m_pTextureCom2;
	Engine::CTransform* m_pTransformCom2;
public:
	static CBoss1* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};
