#pragma once

#include "CGameObject.h"
#include "CMiniGame_Header.h"

namespace Engine
{
	class CTexture;
}

class CMiniGame;
class CMG0_Card;

class CMG0_Deck : public CGameObject
{
protected:
	explicit CMG0_Deck(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_Deck();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject() {}

	_bool			Can_Use(MINIGAME0::MGCARD eCard);
	MINIGAME0::MGCARDCOST	Get_Cost(MINIGAME0::MGCARD eCard);
	_bool			Is_ActiveCard(MINIGAME0::MGCARD eCard);
	_float			Get_MoodRate();

	CMG0_Card*		Get_Hand(_int iIndex) { return m_pHand[iIndex]; }
	MINIGAME0::MGCARDSTATE	Get_State() const { return m_tState; }
	MINIGAME0::MGCARDSTATE	Get_Preview();
	_int			Get_Turn() const { return m_iTurn; }
	_int			Get_Select() const { return m_iSelect; }
	_float			Get_TimeRate() const { return m_fTimeRate; }

	_bool			Is_CardTime() const { return m_bCardTime; }
	_bool			Is_Playing() const { return m_bPlaying; }
	_bool			Is_Complete() const { return m_bComplete; }
	_bool			Is_Finished() const { return m_bFinished; }

	_bool			Is_Notice() const { return m_fNoticeTime > 0.f; }
	const wstring&	Get_Notice() const { return m_strNotice; }

private:
	CMiniGame*		Get_Scene();
	void			Set_Notice(const wstring& strNotice);

	HRESULT			Add_Component();
	void			Ready_Deck();
	void			Shuffle_Deck();
	MINIGAME0::MGCARD	Draw_Card();
	void			Draw_Hand();
	void			Discard_Hand();
	void			Update_Hand();

	void			Start_Turn();
	void			Next_Turn();
	void			Complete();
	void			Finish();

	void			Key_Input();
	void			Select(_int iSlot);
	void			Confirm();
	_bool			Is_HandEmpty();
	_bool			Use_Card(_int iSlot);
	void			Activate_Card(MINIGAME0::MGCARD eCard);
	void			Sync_Attack();
	void			Rest();

	void			Pay_Cost(const MINIGAME0::MGCARDCOST& tCost);
	void			Apply_Effect(MINIGAME0::MGCARD eCard);

	void			Add_Parameter(_int iParameter, _float fAttentionRate = 1.f);
	void			Add_ActiveParameter();
	void			Add_MoodGood(_int iTurn);
	void			Add_MoodPerfect(_int iTurn);
	void			Add_CostHalf(_int iTurn);
	void			Add_Attention(_int iValue);
	void			Add_Stamina(_int iValue);
	void			Add_ExtraAction(_int iCount);
	_int			Recover_CardHp(_int iValue);
	void			Consume_CardHp(_int iCost);
	void			Consume_MoodGood(_int iTurn);
	void			Consume_Attention(_int iValue);
	void			Consume_ExtraAction(_int iCount);
	_int			Apply_CostHalf(_int iCost);

	void			Add_Turn(MINIGAME0::MGSTATUS eStatus, _int iTurn);
	void			Update_Turn(MINIGAME0::MGSTATUS eStatus);
	void			Update_Status();

private:
	vector<MINIGAME0::MGCARD>	m_vecDeck;
	vector<MINIGAME0::MGCARD>	m_vecDiscard;
	CMG0_Card*		m_pHand[MG_CARD_HAND_COUNT]{};
	Engine::CTexture*	m_pCardTex[MINIGAME0::CARD_END]{};

	MINIGAME0::MGCARDSTATE	m_tState;

	_int		m_iTurn = 0;
	_int		m_iSelect = MG_CARD_SELECT_NONE;
	_float		m_fTimeRate = 1.f;
	_bool		m_bCardTime = false;
	_bool		m_bPlaying = false;
	_bool		m_bComplete = false;
	_bool		m_bFinished = false;
	_bool		m_bActionUsed = false;

	wstring		m_strNotice;
	_float		m_fNoticeTime = 0.f;

public:
	static CMG0_Deck* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};
