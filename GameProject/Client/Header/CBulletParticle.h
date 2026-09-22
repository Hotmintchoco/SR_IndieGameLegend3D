#pragma once

#include "CGameObject.h"
#include "Client_Enum.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CTransform;
}

class CBulletParticle : public CGameObject
{
protected:
	explicit CBulletParticle(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBulletParticle();

public:
	HRESULT						Ready_GameObject(const _vec3* pPos, const _vec3* pDir);
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();


private:
	HRESULT			Add_Component();

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTransform* m_pTransformCom;

	_vec3				m_vDir;
	_float				m_fTime;
	_int				m_iColor;

public:
	static CBulletParticle* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pPos, const _vec3* pDir, _int iBulletColor);

private:
	virtual void		Free();
};

