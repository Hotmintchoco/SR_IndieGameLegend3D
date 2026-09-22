#pragma once

#include "CGameObject.h"
#include "Client_Enum.h"
#include "CCustomTex.h"

namespace Engine
{
	class CCustomTex;
	class CTexture;
	class CTransform;
}

class CBulletTrail : public CGameObject
{
protected:
	explicit CBulletTrail(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBulletTrail();

public:
	HRESULT						Ready_GameObject(const _vec3* pPos, const _vec3* pDir, _float fSpeed);
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component(vector<VTXTEX>& vecVtx, vector<INDEX16>& vecIdx);

private:
	Engine::CCustomTex* m_pBufferCom;
	Engine::CTexture*	m_pTextureCom;
	Engine::CTransform* m_pTransformCom;

	_vec3				m_vDir;
	_float				m_fSpeed;

public:
	static CBulletTrail* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pPos, const _vec3* pDir, _float fSpeed);

private:
	virtual void		Free();
};

