#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CTransform;
	class CTexture;
	class CRcTex;
}

class CEnding_Object : public CGameObject
{
protected:
	explicit CEnding_Object(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEnding_Object();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;

private:
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CRcTex* m_pBufferCom = nullptr;

	_float m_fFrame = 0.f;
	_bool m_bStart = false;

public:
	static CEnding_Object* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CEnding_Object* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iText, _vec3 vPos, _vec3 vDst);

	void Set_Dist(_vec3 vDst) { m_vDst = vDst; }
	void Set_Text(_int iText) { m_iText = iText; }
	void Set_Pos(_vec3 vPos);
private:
	virtual void		Free();

	_int m_iText = 0;
	_vec3 m_vDst = { 0.f,0.f,0.f };
};

