#pragma once

#include "CGameObject.h"
#include "Client_Struct.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
	class CCollider;
}

struct TRoomEventCtx;

class CPlayerCamera;

class CMonster : public CGameObject
{
protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonster();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(COLLINFO eCollInfo) override;
	virtual 		void		OnCollisionStay(COLLINFO eCollInfo) override;

	_bool Get_Collision_WithMonster() { return m_bCollision_WithMonster; }

	void Set_Damage(_float fDamage) { m_iHp -= (_int)fDamage; }
	_int Get_Hp() { return m_iHp; }
	_int Get_MaxHp() { return m_iMaxHp; }

protected:
	HRESULT			Add_Component();
	void Set_OnTerrain();
	void Update_HitState(const _float& fTimeDelta);
	void Enable_HitRenderState();
	void Disable_HitRenderState();

	void Chase_Player(const _float& fTimeDelta, _float fSpeed);
	void LookAtPlayer();
	void LookAtPlayer2();

	void Effect_SmallExplode();
	void DropItem();
	void DropItem_Boss();

	// 정민 : OnCollisionStay에서 호출 (몬스터끼리 뭉침 방지 용)
	void CollisionWithMonster(COLLINFO eCollInfo);

	enum GENERAL_MONSTER_STATE { MOVE, JUMP, IDLE };
protected:
	Engine::CRcTex* m_pBufferCom = nullptr;
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CCalculator* m_pCalculatorCom = nullptr;
	Engine::CCollider* m_pColliderCom = nullptr;

	_int m_iMaxHp = 10;
	_int m_iHp = m_iMaxHp;
	_float m_fFrame = 0.f;
	_float m_fHitEffectTime = 0.1f;
	_float m_fHitEffectElapsedTime = 0.f;
	_bool m_bHitState = false;

	_bool m_bDelete = false;
	GENERAL_MONSTER_STATE m_eMonsterState = IDLE;
	_float m_fElapsedTime = 0.f;

	_bool m_bCollision_WithMonster = true;
	_bool m_bRoomCenterLocation = false;
	_vec3 m_vRoomCenterLocation = { 0.f,0.f,0.f };

	/* 성철 */
	CPlayerCamera* m_pBillBoardCamera = nullptr;
	/* --- */

private:
	/* 성철 */
	void OnRoomEvent(const TRoomEventCtx& t);
	/* --- */

public:
	void Set_Pos(_vec3 vPos);
	void Set_Pos(_float fX, _float fY, _float fZ);

	void Get_Pos(_vec3* pPos);

	_float GetCenterX(_float x)
	{
		return 60.f + 15.f * floorf((x - 60.f + 7.5f) / 15.f);
	}

	_float GetCenterZ(_float z)
	{
		return 60.f + 13.f * floorf((z - 60.f + 6.5f) / 13.f);
	}
	void Set_RoomCenterLocation();
public:
	static CMonster* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};

