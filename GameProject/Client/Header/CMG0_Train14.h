#pragma once

#include "CGameObject.h"
#include "CMiniGame_Header.h"

namespace Engine
{
	class CRcTex;
	class CPlyTex;
	class CTexture;
	class CTransform;
}

class CMG0_Train14 : public CGameObject
{
protected:
	explicit CMG0_Train14(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_Train14();

public:
	virtual			HRESULT		Ready_GameObject(_int iLane, _float fSpeed);
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();
	_bool			Is_Warning() const { return m_fWarnTime < MG_TRAIN14_WARN_TIME; }
	void			Render_Train();
	void			Render_Warn();
	void			Render_Strip(_float fX, _float fHalfX, DWORD dwColor);
	void			Crash_Train13();
	void			Collision_Player();

private:
	Engine::CPlyTex*	m_pBufferCom = nullptr;
	Engine::CRcTex*		m_pWarnBufferCom = nullptr;
	Engine::CTransform*	m_pTransformCom = nullptr;
	Engine::CTexture*	m_pTextureCom = nullptr;

	_vec3		m_vPos{ 0.f, 0.f, 0.f };
	_float		m_fSpeed = 0.f;
	_float		m_fWarnTime = 0.f;

public:
	static CMG0_Train14* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iLane, _float fSpeed);

protected:
	virtual void		Free();
};
