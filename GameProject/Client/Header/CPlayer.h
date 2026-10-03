#pragma once

#include "CGameObject.h"
#include "Client_Enum.h"

namespace Engine
{
	class CTransform;
	class CCollider;
	class CTexture;
}

class CPlayerAnimator;
class CPlayerPartTex;
class CPlayerMovement;
class CWeaponSystem;

class CPlayer : public CGameObject
{
protected:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer();

public:
	virtual HRESULT	Ready_GameObject() override;
	virtual _int Update_GameObject(_float fTimeDelta) override;
	virtual void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual void Render_GameObject() override;

	virtual	void OnCollisionEnter(COLLINFO eCollInfo) override;
	virtual	void OnCollisionStay(COLLINFO eCollInfo) override;

	/* HP 관련 */
	void OnHit(CGameObject* pSrcObj);
	void Revive();
	void OnDead();
	void RestoreHP(int iAmount);

	/* 입력 관련 */
	void SetInputEnabled(bool bFlag, float fFixedTime = -1.f);

	inline CTransform* GetTransform() { return m_pTransformCom; }
	inline void SetWeaponSystem(CWeaponSystem* pSystem) { m_pWeaponSystem = pSystem; }

private:
	HRESULT	Add_Component();
	void KeyInput();
	void CursorHandling();

	/* 기본 컴포넌트 */
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CCollider* m_pColliderCom = nullptr;

	/* 캐릭터 애니메이션 관련 */
	CPlayerAnimator* m_pAnimator = nullptr;
	CPlayerPartTex* m_pBufferCom[PP_END] = { nullptr };
	CTransform* m_pBufferTransformCom[PP_END] = { nullptr };
	Engine::CTexture* m_pTextureCom = nullptr;

	/* 캐릭터 작아짐 연출용 */
	float m_fPseudoScale = 1.f;
	float m_fColliderScale = 0.5f;

	/* 캐릭터 기본 스탯 */
	int m_iMaxHP = 12;
	int m_iHP = m_iMaxHP;

	/* 캐릭터 움직임 관련 */
	CPlayerMovement* m_pMovement = nullptr;
	
	/* 피격 관련 */
	bool m_bInvincible = false;
	float m_fInvincibleTime = 1.f;
	float m_fLeftInvincibleTime = 1.f;

	/* 입력 막기 */
	bool m_bInputEnabled = true;
	float m_fLeftInputDisabledTime = 0.f;

	/* 무기 */
	CWeaponSystem* m_pWeaponSystem = nullptr;
	
public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

