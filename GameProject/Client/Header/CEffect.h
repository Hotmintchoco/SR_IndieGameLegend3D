#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CTransform;
}

class CEffect : public CGameObject
{
protected:
	explicit CEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffect();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	HRESULT			Add_Component();

protected:
	Engine::CTransform* m_pTransformCom;
	_float				m_fFrame;
public:
	static CEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	void Set_Pos(_vec3 vPos);
	void Set_Pos(_float fX, _float fY, _float fZ);
	void Set_Scale(_vec3 vPos);
	void Set_Scale(_float fX, _float fY, _float fZ);

public:

protected:
	virtual void		Free();
};

