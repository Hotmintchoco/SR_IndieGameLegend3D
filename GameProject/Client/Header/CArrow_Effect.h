#pragma once

#include "CParticle.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
}

class CArrow_Effect : public CParticle
{
protected:
	explicit CArrow_Effect(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CArrow_Effect();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	HRESULT			Add_Component();
protected:
	Engine::CRcTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
public:
	static CArrow_Effect* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CArrow_Effect* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vDir);
	void Set_Dir(_vec3 vDir) { m_vDir = vDir; }
private:
	_vec3 m_vDir = {};
	_vec3 m_vRandDir = {};
	_int m_iFrame = 0;

protected:
	virtual void		Free();
};

