#pragma once

#include "CGameObject.h"
#include "CMiniGame_Header.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
}

class CMiniGame;

class CMG0_HUD : public CGameObject
{
private:
	enum GUIDESTEP { GUIDE_WAIT, GUIDE_ICON, GUIDE_TUTORIAL, GUIDE_DONE };

	struct HANDSLOT
	{
		MINIGAME0::MGCARD		eCard = MINIGAME0::CARD_END;
		MINIGAME0::MGCARDCOST	tCost;
		_bool		bUsable = false;
	};

	struct TEXTRUN
	{
		wstring		strText;
		_bool		bNumber = false;
		_float		fWidth = 0.f;
	};

	struct STATVIEW
	{
		_int		iActual = 0;
		_int		iTarget = 0;
		_float		fShown = 0.f;
		_bool		bReady = false;
	};

	struct TEXTCMD
	{
		wstring		strText;
		_vec2		vPos{ 0.f, 0.f };
		D3DXCOLOR	Color{ 1.f, 1.f, 1.f, 1.f };
		_bool		bCenter = false;
		_bool		bOutline = true;
		_float		fScale = 1.f;
	};

protected:
	explicit CMG0_HUD(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_HUD();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();
	HRESULT			Ready_Font();

	_float			Get_TextWidth(LPD3DXFONT pFont, const wstring& strText, const _tchar* pGuard);
	vector<TEXTRUN>	Split_Text(const wstring& strText);
	_float			Measure_Text(const wstring& strText);

	void			Update_View(STATVIEW& tView, _int iActual, _int iTarget, _float fTimeDelta);
	_int			Get_Shown(const STATVIEW& tView);
	void			Update_Guide(CMiniGame* pScene, _float fTimeDelta);

	void			Render_Quad(Engine::CTexture* pTexture, _uint iFrame, _float fX, _float fY, _float fSizeX, _float fSizeY, DWORD dwTint);
	void			Render_Rect(_float fX, _float fY, _float fHalfX, _float fHalfY, DWORD dwColor);
	void			Render_Icon(MINIGAME0::MGICON eIcon, _float fX, _float fY, _float fSize, DWORD dwTint = 0xFFFFFFFF);
	void			Render_Icon(MINIGAME0::MGICON eIcon, _float fX, _float fY, _float fSizeX, _float fSizeY, DWORD dwTint = 0xFFFFFFFF);
	void			Render_Text(const wstring& strText, _vec2 vPos, D3DXCOLOR Color, _bool bCenter = false, _bool bOutline = true, _float fScale = 1.f);
	void			Render_Cost(const MINIGAME0::MGCARDCOST& tCost, _float fX, _float fY, D3DXCOLOR Color, _bool bOutline = true);
	void			Render_Value(const STATVIEW& tView, _float fX, _float fY, D3DXCOLOR Color, const wstring& strSuffix = L"");
	void			Flush_Text();
	void			Draw_Run(const vector<TEXTRUN>& vecRun, _vec2 vPos, D3DXCOLOR Color);

	void			Render_Life();
	void			Render_Attack();
	void			Render_Complete();
	void			Render_Vital();
	void			Render_Buff();
	void			Render_Turn();
	void			Render_Hand();
	void			Render_Rest();
	void			Render_Popup();
	void			Render_Notice();
	void			Render_BossHp();
	void			Render_Warning();
	void			Render_Guide();
	void			Render_HandPreview();
	void			Render_Tutorial();
	void			Render_Spotlight(_float fX, _float fY, _float fHalfX, _float fHalfY);
	void			Render_WarningText();
	void			Render_GameOverText();
	void			Render_ClearText();

private:
	Engine::CRcTex*		m_pBufferCom = nullptr;
	Engine::CTexture*	m_pHeartTex = nullptr;
	Engine::CTexture*	m_pHitTex = nullptr;
	Engine::CTexture*	m_pBossFaceTex = nullptr;
	Engine::CTexture*	m_pIconTex[MINIGAME0::ICON_END]{};

	LPD3DXFONT		m_pTextFont = nullptr;
	LPD3DXFONT		m_pNumFont = nullptr;
	LPD3DXSPRITE	m_pSprite = nullptr;
	_float			m_fTextGuard = 0.f;
	_float			m_fNumGuard = 0.f;
	_float			m_fNumOffsetY = 0.f;

	vector<TEXTCMD>	m_vecText;
	DWORD			m_dwMagFilter = D3DTEXF_POINT;
	DWORD			m_dwMinFilter = D3DTEXF_POINT;

	_int		m_iLife = MG_MAX_LIFE;
	_int		m_iHitAlpha = 0;
	_bool		m_bCleared = false;

	STATVIEW	m_tAttack;
	STATVIEW	m_tStatus[MINIGAME0::STATUS_END];
	STATVIEW	m_tExtraActivate;
	STATVIEW	m_tExtraAction;
	STATVIEW	m_tActiveParameter;

	_bool		m_bCardTime = false;
	_bool		m_bPlaying = false;
	_bool		m_bComplete = false;
	_int		m_iTurn = 0;
	_int		m_iSelect = MG_CARD_SELECT_NONE;
	_float		m_fTurnRate = 0.f;

	HANDSLOT	m_tHand[MG_CARD_HAND_COUNT];
	wstring		m_strNotice;

	_bool		m_bWarning = false;
	_int		m_iWarningAlpha = 0;
	_bool		m_bBossHp = false;
	_bool		m_bTimeOver = false;
	_int		m_iWhiteAlpha = 0;
	_float		m_fBossHpRate = 0.f;

	GUIDESTEP	m_eGuide = GUIDE_WAIT;
	_float		m_fGuideDelay = 0.f;
	_float		m_fTutorialStart = 0.f;
	_int		m_iTutorialPage = 0;

public:
	static CMG0_HUD* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};
