#pragma once

#include "CGameObject.h"
#include "Client_Enum.h"
#include "Client_Struct.h"

namespace Engine
{
	class CTransform;
	class CCamera;
}

struct TWeaponLocalInfo
{
	_vec3 vScale{ 1.f, 1.f, 1.f };
	_vec3 vRotation{ 0.f, 0.f, 0.f };
	_vec3 vPosition{ 0.f, 0.f, 0.f };
};

class CMonster;
class CWeaponSystem;

struct TWeaponAnimArgs;

enum class CAMERA_MODE;

class CWeapon : public CGameObject
{
protected:
	explicit CWeapon(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CWeapon();

public:
	virtual	HRESULT Ready_GameObject();
	virtual	_int Update_GameObject(_float fTimeDelta);
	virtual	void LateUpdate_GameObject(_float fTimeDelta);
	virtual	void Render_GameObject() PURE;

	virtual TWeaponOutput DefaultAttack(EInputState ePri, EInputState eSec);
	virtual TWeaponOutput SpecialAttack(EInputState ePri, EInputState eSec) PURE;
	virtual TWeaponOutput StartUltimateAttack(EInputState ePri, EInputState eSec) PURE;
	virtual TWeaponOutput UpdateUltimateAttack(EInputState ePri, EInputState eSec) PURE;
	virtual TWeaponOutput EndUltimateAttack(EInputState ePri, EInputState eSec) PURE;

	void UpdateAnimationArgs(const TWeaponAnimArgs& t);

	inline bool IsOnCoolTime() { return m_bIsCoolTime; }
	inline float GetSpecialAtkGaugeConsume() { return m_fGaugeConsumePerSpecialAtk; }
	inline CTransform* GetTransform() { return m_pTransformCom; }
	TWeaponLocalInfo GetLocalInfo(CAMERA_MODE eMode);
	void UpdateLocalTransform(const TWeaponLocalInfo& tInfo);
	inline void SetSystem(CWeaponSystem* pSystem) { m_pSystem = pSystem; }

protected:
	HRESULT	Add_Component();
	void SyncTransformToCamera(CCamera* pCamera);
	void UpdateBulletShotPos(const _matrix& matWorld, const _matrix& matCamera);
	void CheckCoolTime(const _float& fTimeDelta);
	void Animation(const _float fTimeDelta);
	void StartShotAnimation();
	void ShotSingleBullet();
	void ShotSingleBullet(const _vec3& vToward);

	Engine::CTransform* m_pTransformCom = nullptr;

	/* 1/3인칭 로컬 오프셋 */
	TWeaponLocalInfo m_tLocalFView;
	TWeaponLocalInfo m_tLocalTView;
	_vec3 m_vMuzzlePositionLocal{0.f, 0.f, 0.f};

	/* 기본 공격 */
	float m_fShootInterval = 0.2f; // 애니메이션 시간은 여기에 맞추기
	float m_fCoolTimeLeft = 0.0f;
	bool m_bIsCoolTime = false;
	
	/* 발사 목적지 */
	float m_fTargetDistance = 10.f;
	_vec3 m_vBulletFrom{ 0.f, 0.f, 0.f };
	_vec3 m_vBulletTo{ 0.f, 0.f, 0.f };

	/* 특수 공격 */
	float m_fGaugeConsumePerSpecialAtk = 0.04f;
	float m_fSpecialAtkInterval = 0.5f;

	/* 궁극기 */
	bool m_bOnUltimateAttack = false;

	/* 애니메이션 */
	bool m_bOnMoveAnimation = false;
	bool m_bOnSprint = false;
	float m_fTimeAfterMove = 0.f; // 매개변수 t 
	float m_fMoveAnimationFrequency = 1.f; // 애니메이션이 초당 몇 번
	float m_fHorizontalMove = 0.02f; // 가로로 움직이는 거리
	float m_fQuadraticA = 0.02f; // 이차함수 제곱항 계수
	bool m_bShotAnimation = false;
	float m_fMaxRecoilAngle = -20.f;
	float m_fRecoilDamping = 2.f; // 반동 감쇠. 0으로 갈수록 직선, 값이 커질수록 아래로 굽은 곡선
	float m_fTimeAfterShot = 0.f;
	bool m_bSpecialAttackSwitchOn = false;

	/* 정보 전달 */
	CWeaponSystem* m_pSystem = nullptr;

protected:
	virtual void Free() override;
};

