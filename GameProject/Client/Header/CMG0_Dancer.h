#pragma once

#include "CGameObject.h"
#include "CMiniGame_Header.h"

namespace Engine
{
	class CTexture;
	class CTransform;
}

class CMG0_DancerPart;
class CMG0_Deck;
class CMG0_Player;
class CMG0_Dialogue;
class CMG0_Sunglass;

class CMG0_Dancer : public CGameObject
{
private:
	enum PART { PART_BODY, PART_HEAD, PART_LARM, PART_RARM, PART_LLEG, PART_RLEG, PART_END };
	enum STATE { STATE_WAIT, STATE_ENTER, STATE_IDLE, STATE_DANCE, STATE_BOARD, STATE_END };

protected:
	explicit CMG0_Dancer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_Dancer();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

public:
	_bool			Is_Boarded() const { return STATE_END == m_eState; }

private:
	HRESULT			Add_Component();
	HRESULT			Ready_Part();
	void			Update_State(CMG0_Dialogue* pDialogue, CMG0_Deck* pDeck, CMG0_Player* pPlayer, _float fTimeDelta);
	void			Update_Part(_float fTimeDelta);
	void			Update_Sunglass(CMG0_Deck* pDeck, _float fTimeDelta);
	MINIGAME0::MGDANCEBEAT	Get_DanceBeat();

private:
	CMG0_DancerPart*	m_pPart[PART_END]{};
	CMG0_Sunglass*		m_pSunglass = nullptr;
	Engine::CTexture*	m_pTextureCom = nullptr;
	Engine::CTransform*	m_pTransformCom = nullptr;

	STATE		m_eState = STATE_WAIT;
	_vec3		m_vPos = MG_DANCER_POS;
	_float		m_fHeight = MG_DANCER_ENTER_HEIGHT;
	_float		m_fStateTime = 0.f;
	_float		m_fDanceTime = 0.f;
	_bool		m_bSunglass = false;

public:
	static CMG0_Dancer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};
