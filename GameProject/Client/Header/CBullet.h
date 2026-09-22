#pragma once

#include "CGameObject.h"
#include "Client_Enum.h"
#include "CBulletTrail.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CTransform;
	class CCollider;
	class CSphereCollider;
}

class CBullet : public CGameObject
{
protected:
	explicit CBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBullet();

public:
	HRESULT						Ready_GameObject(const _vec3* pPos, const _vec3* pDir);
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(CGameObject* pOther) override;

public:
	_int Get_Damage() { return m_iBulletDmg; }
private:
	HRESULT			Add_Component();

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CCollider* m_pColliderCom;

	_float				m_fSpeed;
	_vec3				m_vDir;
	BULLETID			m_iBulletID;
	_int				m_iBulletDmg;
	_float				m_fBulletLife;
	CBulletTrail*		m_pBulletTrail;


public:
	static CBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pPos, const _vec3* pDir);

private:
	virtual void		Free();
};

