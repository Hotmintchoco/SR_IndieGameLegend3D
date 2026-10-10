#pragma once

#include "CComponent.h"
#include "Client_Enum.h"
#include "CEventDelegate.h"

namespace Engine
{
	class CTransform;
}

class CPlayerPartTex;

struct TPlayerBuffer
{
	CPlayerPartTex* pBuffer = nullptr;
	CTransform* pTransform = nullptr;

	_vec3 vInitPos{};
	_vec3 vInitRot{};
};

struct TAnimPose
{
	TAnimPose() { Reset(); }
	_vec3 vRot[TP_END];
	void Reset() { ZeroMemory(&vRot, sizeof(_vec3) * TP_END ); }
};

struct TActionPose
{
	TActionPose() { Reset(); }
	_vec3 vRot[TP_END];
	bool bMask[TP_END];
	void Reset()
	{
		ZeroMemory(&vRot, sizeof(_vec3) * TP_END);
		ZeroMemory(&bMask, sizeof(bool) * TP_END);
	}
};

struct TLocoParam
{
	float fFreq = 0.f;
	float fLegAmp = 0.f;
	float fArmAmp = 0.f;
};

struct TShootParam
{
	float fHoldTime = 0.6f;	// 마지막 발사 후 팔을 들고 있는 시간
	float fKick = 25.f;		// 반동으로 추가로 들리는 각도 (degree)
	float fAttack = 0.04f;		// 반동 최고점까지 걸리는 시간
	float fRecover = 15.f;		// 반동 복귀 감쇠 속도
};

enum class EEase : uint8_t { E_LINEAR, E_IN, E_OUT, E_INOUT };

/* 1인칭 양팔에만 적용 */
struct TFViewKey
{
	float fTime;
	_vec3 vCamLocal;
	_vec3 vRotDegree;
	EEase eEase = EEase::E_INOUT;
};

class CPlayerAnimator : public CComponent
{
protected:
	explicit CPlayerAnimator(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayerAnimator();

public:
	virtual _int Update_Component(_float fTimeDelta) override;
	virtual void LateUpdate_Component() override;
	virtual CComponent* Clone() { assert(0); return nullptr; };

	void PlayAction(EPlayerActionState eAction);
	void PlayLocomotion(EPlayerLocomotionState eLoco = EPlayerLocomotionState::NONE);

	void TransformPropagation(const _matrix& matRootWorld);
	void TransformFPPropagation(const _matrix& matCamWorld);

	void SetBuffer(const array<TPlayerBuffer, TP_END>& tBuffer);
	inline void SetRootTransform(CTransform* pTransform) { m_pRootTransform = pTransform; }
	inline void SetFPTransform(const array<CTransform*, FP_END>& arrTransform) { m_arrFPTransform = arrTransform; }

	void RenderDebugTransform();

	inline bool OnAction() { return m_eAction != EPlayerActionState::NONE; }

	CEventDelegate<EPlayerActionState> m_OnActionFinished;

	/* 1인칭 애니메이션 여부를 판별하여 플레이어 측에서 팔 버퍼를 띄우기 위함 */
	bool IsFPPartVisible();

private:
	void UpdateLocomotion(float fTimeDelta);
	void UpdateAction(float fTimeDelta);
	void ApplyPose();
	static TLocoParam GetLocoParam(EPlayerLocomotionState eLoco);
	void SetInitialTransform();

	// Root
	// ├─ vBody
	// │  ├─ vHead
	// │  ├─ vLArm
	// │  └─ vRArm
	// ├─ vLLeg
	// └─ vRLeg

	/* 버퍼 저장용 */
	const static array<int, TP_END> s_arrParent;
	const static array<PLAYERTPPART, TP_END> s_arrUpdateOrder;
	static const char* s_szPartName[TP_END];
	array<TPlayerBuffer, TP_END> m_arrBuffer = {};
	CTransform* m_pRootTransform = nullptr;

	/* 1인칭 애니메이션 용 */
	array<CTransform*, 2> m_arrFPTransform{};

	/* 애니메이션 상태 */
	EPlayerActionState m_eAction = EPlayerActionState::NONE;
	EPlayerLocomotionState m_eLoco = EPlayerLocomotionState::IDLE;
	TAnimPose m_tPose;

	/* 로코모션 */
	TLocoParam m_tCurLoco;
	float m_fPhase = 0.f;

	/* 액션 */
	TActionPose m_tActionPose;
	float m_fActionTime = 0.f;
	float m_fActionWeight = 0.f;

	/* 액션 : 사격 */
	void SampleFire(float fTime);
	static constexpr TShootParam s_tShootParam{};

	/* 액션 : 팔 들기 */
	void SampleShotGunUltimate(float fTime);
	static constexpr float s_fRipperDuration = 3.f;

	/* 액션 : 전술 조준경 */
	void SampleKeys(const TFViewKey* pKeys, int iCount, float fTime, CTransform* pTarget);
	void SampleRapidGunUltimateStart(float fTime);

	/* 액션 : 석양 */
	void SampleLiminalGunUltimateStart(float fTime);
	void SampleLiminalGunUltimateLoop(float fTime);
	void SampleLiminalGunUltimateEnd(float fTime);

	/* 애니메이션 디버그용 */
	void DrawTransformEditor(const char* szName, CTransform* pTransform,
		const _vec3* pInitPos, const _vec3* pInitRot,
		bool bCopyAsKey);
	float m_fDebugScrubTime = 0.f;
	bool m_bAnimPause = false;
	int m_iDebugClip = 0;

	constexpr static int TP_ROOT = -1;
	static constexpr float s_fForward = -1.f; /* LH 기준 팔 +x 회전 */
	static constexpr float s_fLocoBlendSpeed = 10.f; /* 로코모션이 바뀔 때 목표 Param까지 멤버 Param이 쫓아가는 속도 */
	static constexpr float s_fActionBlendSpeed = 20.f; /* 액션이 시작될/끝날 때 목표 Param까지 멤버 Param이 쫓아가는 속도 */

public:
	static CPlayerAnimator* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};

