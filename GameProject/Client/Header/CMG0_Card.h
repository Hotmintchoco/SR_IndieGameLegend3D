#pragma once

#include "CGameObject.h"
#include "CMiniGame_Header.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
}

class CMG0_Card : public CGameObject
{
protected:
	explicit CMG0_Card(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_Card();

public:
	virtual			HRESULT		Ready_GameObject(MINIGAME0::MGCARD eCard, _int iSlot, Engine::CTexture* pTexture);
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	MINIGAME0::MGCARD	Get_Card() const { return m_eCard; }
	void			Set_Usable(_bool bUsable) { m_bUsable = bUsable; }
	void			Set_Selected(_bool bSelected) { m_bSelected = bSelected; }

private:
	HRESULT			Add_Component(Engine::CTexture* pTexture);

private:
	Engine::CRcTex*		m_pBufferCom = nullptr;
	Engine::CTexture*	m_pTextureCom = nullptr;

	MINIGAME0::MGCARD	m_eCard = MINIGAME0::CARD_END;
	_vec2		m_vPos{ 0.f, 0.f };
	_bool		m_bUsable = true;
	_bool		m_bSelected = false;

public:
	static CMG0_Card* Create(LPDIRECT3DDEVICE9 pGraphicDev, MINIGAME0::MGCARD eCard, _int iSlot, Engine::CTexture* pTexture);

protected:
	virtual void		Free();
};
