#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CPlyTex;
	class CTransform;
	class CTexture;
}

class CWeapon : public CGameObject
{
protected:
	explicit CWeapon(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CWeapon();

public:
	virtual	HRESULT Ready_GameObject();
	virtual	_int Update_GameObject(const _float& fTimeDelta);
	virtual	void LateUpdate_GameObject(const _float& fTimeDelta);
	virtual	void Render_GameObject();

	/* 에너지 아이템 획득 */
	void GainEnergy();

private:
	HRESULT	Add_Component();
	void SyncTransformToCamera();
	void TryShoot();
	void UpdateLocalTransform(const _vec3& vScale, const _vec3& vRotation, const _vec3& vTransition);
	void RenderEditorPanel();
	void UltimateAttack();
	void Animation(const _float fTimeDelta);
	void StartShotAnimation();
	void GetKeyInput();

	Engine::CPlyTex* m_pBufferCom = nullptr;
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;

	/* 카메라 시점 기준 로컬 오프셋 */
	_vec3 m_vScaleLocal{0.3f, 0.3f, 0.45f};
	_vec3 m_vPositionLocal{0.13f, -0.33f, 0.35f};
	_vec3 m_vRotationLocal{ -1.f, -2.f, 0.f };
	_vec3 m_vMuzzlePositionLocal{0.0f, 0.4f, 0.7f};

	/* 발사 쿨타임 */
	float m_fShootInterval = 0.1f; // 애니메이션 시간은 여기에 맞추기
	float m_fCoolTimeLeft = 0.0f;
	bool m_bIsCoolTime = false;

	/* 발사 목적지 */
	float m_fTargetDistance = 10.f;
	_vec3 m_vBulletFrom{ 0.f, 0.f, 0.f };
	_vec3 m_vBulletTo{ 0.f, 0.f, 0.f };

	/* 특수 공격 */
	bool m_bSpecialAttackSwitchOn = false;
	float m_fSpecialAtkGauge = 0.f;
	
	/* 궁극기 */
	float m_fUltimateAtkGauge = 0.f;
	float m_bIsUltimateAttackReady = false;

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


public:
	static CWeapon* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

