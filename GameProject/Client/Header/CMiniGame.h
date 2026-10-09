#pragma once

#include "CScene.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
}

class CMiniGame : public CScene
{
private:
	struct MGVTX
	{
		_vec3	vPos;
		DWORD	dwColor;
	};

	struct MGOBJ
	{
		_vec3	vPos{ 0.f, 0.f, 0.f };
		_vec3	vVel{ 0.f, 0.f, 0.f };
		_float	fTimer = 0.f;
		_float	fLife = 0.f;
		_float	fCool = 0.f;
		_float	fPhase = 0.f;
		_float	fScale = 1.f;
		_float	fFlash = 0.f;
		_int	iHp = 1;
	};

	struct MGGATE
	{
		_vec3	vPos{ 0.f, 0.f, 0.f };
		_int	iType = 0;
		_int	iPairId = 0;
	};

private:
	explicit CMiniGame(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMiniGame();

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(_float fTimeDelta);
	virtual			void		LateUpdate_Scene(_float fTimeDelta);
	virtual			void		Render_Scene();

	virtual HRESULT Add_GameObject(const wstring& pObjTag, CGameObject* pGameObject) override { return S_OK; }

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag) { return S_OK; }
	HRESULT			Ready_UI_Layer(const _tchar* pLayerTag) { return S_OK; }
	HRESULT			Ready_Resource();

	void			Build_PlaneMesh();
	void			Add_Quad(vector<MGVTX>& vecMesh, const _vec3& vCenter, _float fHalfX, _float fHalfY, DWORD dwColor);
	void			Reset_Game();
	void			Update_Player(_float fTimeDelta);
	void			Update_Bullets(_float fTimeDelta);
	void			Update_Enemies(_float fTimeDelta);
	void			Update_Explosions(_float fTimeDelta);
	void			Update_Gates(_float fTimeDelta);
	void			Spawn_Gates();
	void			Apply_Gate(_int iType);
	void			Update_Camera();
	void			Spawn_Explosion(const _vec3& vPos, _float fScale);
	void			Hit_Player();

	void			Render_Sprite(CTexture* pTexture, _uint iFrame, const _vec3& vPos, _float fScale, DWORD dwTint);
	void			Render_ScreenQuad(CTexture* pTexture, _uint iFrame, _float fX, _float fY, _float fSizeX, _float fSizeY, DWORD dwTint);
	void			Render_Gates();
	void			Render_GateText();
	void			Render_HUD();

public:
	static CMiniGame* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free();

private:
	CRcTex*			m_pBufferCom = nullptr;
	CTexture*		m_pEnemyTex = nullptr;
	CTexture*		m_pEnemyBulletTex = nullptr;
	CTexture*		m_pBulletTex = nullptr;
	CTexture*		m_pExplodeTex = nullptr;
	CTexture*		m_pHeartTex = nullptr;
	CTexture*		m_pHitTex = nullptr;

	vector<MGVTX>	m_vecPlaneMesh;

	vector<MGOBJ>	m_vecBullets;
	vector<MGOBJ>	m_vecEnemies;
	vector<MGOBJ>	m_vecEnemyBullets;
	vector<MGOBJ>	m_vecExplosions;
	vector<MGGATE>	m_vecGates;

	_matrix			m_matView;
	_matrix			m_matProj;
	_matrix			m_matBill;

	_vec3			m_vPlayerPos{ 0.f, 2.f, 0.f };
	_float			m_fRoll = 0.f;
	_float			m_fPitch = 0.f;
	_float			m_fFireCool = 0.f;
	_float			m_fSpawnTimer = 0.f;
	_float			m_fPlayTime = 0.f;
	_float			m_fInvincible = 0.f;
	_float			m_fRestartTimer = 0.f;
	_float			m_fAnimTime = 0.f;
	_float			m_fGateTimer = 0.f;
	_float			m_fFireInterval = 0.12f;
	_float			m_fClearTimer = 3.f;
	_int			m_iGatePairId = 0;
	_int			m_iDamage = 1;
	_int			m_iBulletCount = 1;
	_int			m_iLife = 5;
	_int			m_iKillCount = 0;

	_bool			m_bGameOver = false;
};
