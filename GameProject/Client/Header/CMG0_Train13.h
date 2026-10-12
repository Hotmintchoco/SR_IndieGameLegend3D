#pragma once

#include "CGameObject.h"
#include "CMiniGame_Header.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CTransform;
}

class CMG0_Train13 : public CGameObject
{
protected:
	explicit CMG0_Train13(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_Train13();

public:
	virtual			HRESULT		Ready_GameObject(_int iLane, _float fSpeed, Engine::CTexture** ppTexture);
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component(Engine::CTexture** ppTexture);
	void			Ready_Face();
	void			Set_Face(MINIGAME0::MGTRAINFACE eFace, const _vec3& vRight, const _vec3& vUp, const _vec3& vCenter);
	void			Collision_Player();

private:
	Engine::CRcTex*		m_pBufferCom = nullptr;
	Engine::CTransform*	m_pTransformCom = nullptr;
	Engine::CTexture*	m_pTextureCom[MINIGAME0::TRAIN_END]{};

	_matrix		m_matFace[MINIGAME0::TRAIN_END];
	_vec3		m_vPos{ 0.f, 0.f, 0.f };
	_float		m_fSpeed = 0.f;

public:
	static CMG0_Train13* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iLane, _float fSpeed, Engine::CTexture** ppTexture);

protected:
	virtual void		Free();
};
