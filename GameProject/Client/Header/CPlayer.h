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
class CSocket;
class CPlayerCamera;
class CCubeNet;

enum class CAMERA_MODE;

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
	void LockInput(EPlayerInputChannel e);
	void LockInputFor(EPlayerInputChannel e, float fDuration);
	void UnlockInput(EPlayerInputChannel e);
	bool IsInputAllowed(EPlayerInputChannel e);
	/* 이동 입력이 잠기면 마지막 걷기 커맨드/애니메이션이 그대로 남으므로, 잠그는 쪽에서 불러 제자리 아이들로 되돌림 */
	void ResetToIdle();

	/* 가짜 스케일 */
	void SetPseudoScale(float fScale);
	void SetPseudoScale(float fScale, float fLerpTime);
	inline float GetPseudoScale() { return m_fPseudoScale; }

	/* 직교 미니게임 : 충돌체 크기 / 수평 최대 속도 계수 (1이면 원상태) */
	void SetMiniGameBodyCoef(float fCoef);

	inline CTransform* GetTransform() { return m_pTransformCom; }
	void SetWeaponSystem(CWeaponSystem* pSystem);
	inline CWeaponSystem* GetWeaponSystem() { return m_pWeaponSystem; }
	CSocket* GetSocket(const wstring& wstrName);
	inline void SetCamera(CPlayerCamera* pCamera) { m_pCamera = pCamera; }
	inline CPlayerMovement* GetMovement() { return m_pMovement; }

	/* 카메라 뷰 */
	void SyncCameraYaw();
	/* 기준 yaw를 0도로 고정 (직교 미니게임용) : 고정하는 순간 입력 방향도 0도로 초기화 */
	void SetYawFixedToZero(bool bFlag);

	/* 직교 미니게임 관련 */
	void UpdateOrthoObjectCandidate(CCubeNet* pObj);
	bool IsGrabbingOrthoObject() { return m_bIsGrabbingOrthoObject; }
	CCubeNet* GetOrthoObjectCandidate() { return m_pInteractingOrthoObject; }
	/* 페이즈 정리 시 호출 : 들고 있던 물체를 놓고 후보 포인터도 비움 */
	void ResetOrthoInteraction();
	void UpdateOrthoMiniGameInput();
	void UpdateOrthoMiniGameStatus(float fTimeDelta);

private:
	HRESULT	Add_Component();
	void UpdateInput();
	void UpdateWeaponInput();

	/* 카메라 뷰 */
	CPlayerCamera* m_pCamera = nullptr;
	void OnCameraViewChanged(const CAMERA_MODE& Ctx);
	CAMERA_MODE m_ePlayerCamMode; /* 결국 매 프레임 동작해야 하는 것들이 많아 캐싱... */
	float m_fInputYaw = 0.f;
	bool m_bYawFixedToZero = false;

	/* 기본 컴포넌트 */
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CCollider* m_pColliderCom = nullptr;

	/* 캐릭터 애니메이션 관련 */
	void OnActionAnimationFinished(const EPlayerActionState& Ctx);
	CPlayerAnimator* m_pAnimator = nullptr;
	CPlayerPartTex* m_pBufferCom[TP_END] = { nullptr };
	CTransform* m_pTPBufferTransformCom[TP_END] = { nullptr };
	CTransform* m_pVisualRootTransform = nullptr;
	CTransform* m_pFPBufferTransformCom[FP_END] = { nullptr }; /* 왼팔과 오른팔 */
	bool m_bInputYawIgnored = false;
	CTransform* m_pAnimRootTransform = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;

	/* 1/3인칭에 따른 무기 위치 변경용 소켓 */
	CSocket* m_pLHandSocket = nullptr;
	CSocket* m_pRHandSocket = nullptr;

	/* 캐릭터 작아짐 연출용 */
	float m_fPseudoScale = 1.f;
	float m_fColliderScale = 0.5f;
	float m_fMiniGameBodyCoef = 1.f;
	bool m_bLerpPseudoScale = false;
	float m_fLerpTime = 1.f;
	float m_fTimeAfterLerp = 0.f;
	float m_fInitLerpScale = 1.f;
	float m_fTargetLerpScale = 1.f;

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
	_uint m_iInputLockCount[PIC_END] = {};
	float m_fInputLockTime[PIC_END] = {};

	/* 무기 */
	CWeaponSystem* m_pWeaponSystem = nullptr;

	/* 직교 미니게임 관련 */
	CCubeNet* m_pInteractingOrthoObject = nullptr;
	CSocket* m_pOrthoSocket = nullptr;
	bool m_bIsGrabbingOrthoObject = false;

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

