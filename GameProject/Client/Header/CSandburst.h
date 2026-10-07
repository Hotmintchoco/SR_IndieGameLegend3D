#pragma once

#include "CParticle.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
}

class CSandburst : public CParticle
{
protected:
	explicit CSandburst(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSandburst();

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
	static CSandburst* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CSandburst* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

	void Set_OriginPos(_vec3 vPos) { m_vOriginPos = vPos; }
	void LookAtPlayer2();
private:
	_vec3 m_vOriginPos{ 0.f,0.f,0.f };

protected:
	virtual void		Free();
};

