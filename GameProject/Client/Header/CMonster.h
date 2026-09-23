#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
	class CCollider;
}

struct TRoomEventCtx;

class CMonster : public CGameObject
{
protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonster();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();


	virtual			void		OnCollisionEnter(CGameObject* pOther) override;
	void Enable_HitRenderState();
	void Disable_HitRenderState();
protected:
	HRESULT			Add_Component();
	void Set_OnTerrain();

protected:
	Engine::CRcTex* m_pBufferCom = nullptr;
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CCalculator* m_pCalculatorCom = nullptr;
	Engine::CCollider* m_pColliderCom = nullptr;

	_int m_iHp;
	_float m_fFrame;
	_float m_fHitEffectDuration;
	_float m_fHitEffectTime;
	_bool m_bHitState;

	_bool m_bDelete = false;

private:
	/* 성철 */
	void OnRoomEvent(const TRoomEventCtx& t);
	/* --- */

public:
	static _uint iMonsterIdx;
	void Set_Pos(_vec3 vPos);
	void Set_Pos(_float fX, _float fY, _float fZ);
public:
	static CMonster* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};

