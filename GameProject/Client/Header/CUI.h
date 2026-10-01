#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CUI : public CGameObject
{
protected:
	explicit CUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

public:
	void			Set_Pos(const _vec2& vPos);
	void			Set_Pos(_float fX, _float fY, _float fZ);
	void			Set_Size(const _vec2& vSize);
	void            Set_Texture(const _uint& iIndex);
	void			Set_OnSwitch(_bool bFlag) { m_bOnSwitch = bFlag; }

	void			Set_SyncSwitchToActive(_bool bFlag) { m_bSyncSwitchToActive = bFlag; }
	_bool			Get_SyncSwitchToActive() { return m_bSyncSwitchToActive; }

protected:
	HRESULT			Add_Component();

protected:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	_vec3				m_vPos;
	_vec2				m_vSize;
	_float				m_fFrame;
	_bool				m_bOnSwitch;
	_bool				m_bSyncSwitchToActive; // true이면 m_bOnSwitch와 m_bIsActive를 동기화함

	wstring				m_wstrTextureTag;

public:
	static CUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrTextureTag);

protected:
	virtual void		Free();
};

