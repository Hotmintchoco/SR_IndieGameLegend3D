#pragma once

#include "CEffect.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CTransform;
}

class CFrustumExplodeEffect : public CEffect
{
protected:
	explicit CFrustumExplodeEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFrustumExplodeEffect();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	void BillBoard();

	Engine::CRcTex* m_pBufferCom = nullptr;

	float m_fFrameInterval = 0.1f;
	float m_fSingleFrameAccTime = 0.f;
	int m_iTotalFrameCount = -1;
	int m_iCurrentFrame = 0;

public:
	static CFrustumExplodeEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CFrustumExplodeEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale);


private:
	virtual void		Free();
};

