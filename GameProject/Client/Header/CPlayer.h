#pragma once

#include "CGameObject.h"
#include "Client_Enum.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
	class CCollider;
	class CSphereCollider;
	class CPlyTex;
}

class CPlayerAnimator;
class CPlayerPartTex;

class CPlayer : public CGameObject
{
#define	GRAVCONST		60.f
protected:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(COLLINFO eCollInfo) override;
	virtual			void		OnCollisionStay(COLLINFO eCollInfo) override;
	void						Hit(CGameObject* pOther);	// 히트백 적용 안할 시 nullptr 넣어주세요
	void						Freeze()				{ m_fFreezeTimer += 3000.f; } // 플레이어 상호작용 키 막기
	void						Freeze(_float fTime)	{ m_fFreezeTimer += fTime; }
	void						Unfreeze()				{ m_fFreezeTimer = 0.f; }
	void						GiveInvTime(_float fInvTime) { m_fInvTime += fInvTime; }
	void						ClearInvTime() { m_fInvTime = 0.f; }

private:
	HRESULT			Add_Component();
	void			Key_Input(const _float& fTimeDelta);
	void			Mouse_Move();
	void			Mouse_Fix();
	_vec3			Picking_OnTerrain();

	void			RenderImGui();
	Engine::CCollider*	Find_OtherCollider(CGameObject* pOther);
	void			MonsterCollision(CGameObject* pOther, Engine::CCollider* pOtherCollider);
	void			Apply_Knockback(CGameObject* pAttacker);
	void			Update_Knockback(const _float& fTimeDelta);

	void			Update_HPUI();


private:
	Engine::CTransform*			m_pTransformCom;
	Engine::CCalculator*		m_pCalculatorCom;
	Engine::CCollider*			m_pColliderCom;

	CPlayerPartTex* m_pBufferCom[PP_END] = { nullptr };
	CTransform* m_pBufferTransformCom[PP_END] = { nullptr };
	Engine::CTexture* m_pTextureCom = nullptr;

	CPlayerAnimator* m_pAnimator = nullptr;

private:

	_bool		m_bFix;
	_bool		m_bCheck;
	_int		m_iHP;
	_int		m_iMaxHP;
	_float		m_fInvTime;
	_bool		m_bDeathState;
	_float		m_fRespawnTimer;
	_float		m_fFreezeTimer;

	_vec3		m_vKnockbackDir;		
	_float		m_fKnockbackSpeed;		

public:
	static	CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	void	GetItem(ITEMID iItemID);
	void	UpdateHP(_int iAmount);
	void	Die();
	void	Respawn();

private:
	virtual void		Free();
};

