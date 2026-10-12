#pragma once

#include "CScene.h"
#include "CSoundMgr.h"
#include "CMiniGame_Header.h"

class CMG0_Player;
class CMG0_Deck;
class CMG0_Dancer;
class CMG0_Dialogue;
class CMG0_Herobrine;

namespace Engine
{
	class CTexture;
}

class CMiniGame : public CScene
{
private:
	explicit CMiniGame(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMiniGame();

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(_float fTimeDelta);
	virtual			void		LateUpdate_Scene(_float fTimeDelta);
	virtual			void		Render_Scene() override {}

	virtual			void		OnExit() override;

	virtual HRESULT Add_GameObject(const wstring& pObjTag, CGameObject* pGameObject) override;

	void			OnPlayerDead();
	inline CMG0_Player*	Get_Player() { return m_pPlayer; }
	inline CMG0_Deck*	Get_Deck() { return m_pDeck; }
	inline CMG0_Dancer*	Get_Dancer() { return m_pDancer; }
	inline CMG0_Dialogue*	Get_Dialogue() { return m_pDialogue; }
	inline CMG0_Herobrine*	Get_Boss() { return m_pBoss; }
	inline _float	Get_SceneTime() { return m_fSceneTime; }
	inline _bool	Is_Cleared() { return m_fClearTimer < MG_CLEAR_SHOW_TIME; }
	_uint			Get_TileFrame();
	_float			Get_WhiteRate();
	inline _bool	Is_TimeOver() { return m_bTimeOver; }
	void			Clear_Enemy();

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_Texture();
	HRESULT			Ready_UI_Layer(const _tchar* pLayerTag);

	void			Update_BGM();
	void			Spawn_Train(_float fTimeDelta);
	void			Update_Camera();

public:
	static CMiniGame* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free();

private:
	CMG0_Player*	m_pPlayer = nullptr;
	CMG0_Deck*		m_pDeck = nullptr;
	CMG0_Dancer*	m_pDancer = nullptr;
	CMG0_Dialogue*	m_pDialogue = nullptr;
	CMG0_Herobrine*	m_pBoss = nullptr;
	Engine::CTexture*	m_pTrainTex[MINIGAME0::TRAIN_END]{};

	_float			m_fSceneTime = 0.f;
	_float			m_fGameOverTimer = 0.f;
	_float			m_fClearTimer = MG_CLEAR_SHOW_TIME;
	_float			m_fTrainTime = 0.f;
	_float			m_fTrainTimer = MG_TRAIN_START_DELAY;
	_float			m_fCamRate = 0.f;
	_float			m_fWhiteTime = 0.f;
	_int			m_iSafeLane = MG_LANE_COUNT / 2;

	_bool			m_bBGMStarted = false;
	_bool			m_bTimeOver = false;
	CSoundMgr::SOUND_HANDLE	m_hBGM = CSoundMgr::INVALID_SOUND;
};
