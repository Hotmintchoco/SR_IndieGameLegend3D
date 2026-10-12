#pragma once

#include "CGameObject.h"
#include "CMiniGame_Header.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
}

class CMG0_Dialogue : public CGameObject
{
private:
	enum MARK { MARK_TILDE = 0x0334, MARK_STROKE_SHORT, MARK_STROKE_LONG, MARK_SLASH_SHORT, MARK_SLASH_LONG, MARK_END };

	struct GLYPH
	{
		_tchar		cChar = 0;
		_tchar		cMark = 0;
		_vec2		vPos{ 0.f, 0.f };
		_float		fWidth = 0.f;
	};

protected:
	explicit CMG0_Dialogue(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_Dialogue();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

public:
	_bool			Is_Finished() const { return m_bFinished; }

private:
	HRESULT			Add_Component();
	void			Ready_Page(size_t iPage);
	void			Next_Page();
	void			Show_All();
	void			Update_Text(_float fTimeDelta);

	void			Render_Quad(Engine::CTexture* pTexture, _uint iFrame, _float fX, _float fY, _float fHalfX, _float fHalfY);
	void			Render_Text();
	void			Render_Mark(const GLYPH& tGlyph);
	void			Render_Line(_float fX, _float fY, _float fHalfX, _float fHalfY, _float fAngle);

	_bool			Is_Speaking() const { return m_iVisibleWord < m_vecWordEnd.size(); }
	_bool			Is_Mark(_tchar cChar) const { return cChar >= MARK_TILDE && cChar < MARK_END; }
	_bool			Is_Space(const GLYPH& tGlyph) const { return L' ' == tGlyph.cChar; }
	_float			Get_Width(_tchar cChar) const { return cChar < 0x0100 ? MG_DIALOGUE_FONT_SIZE * 0.5f : MG_DIALOGUE_FONT_SIZE; }

private:
	Engine::CRcTex*		m_pBufferCom = nullptr;
	Engine::CTexture*	m_pWindowTex = nullptr;
	Engine::CTexture*	m_pPortraitTex = nullptr;

	vector<GLYPH>		m_vecGlyph;
	vector<size_t>		m_vecWordEnd;

	size_t		m_iPage = 0;
	size_t		m_iVisibleWord = 0;
	size_t		m_iVisibleGlyph = 0;
	_uint		m_iPortraitFrame = 1;
	_float		m_fTextTime = 0.f;
	_float		m_fPageTime = 0.f;
	_float		m_fPortraitTime = 0.f;
	_bool		m_bFinished = false;

public:
	static CMG0_Dialogue* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};
