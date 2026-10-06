#pragma once

#include "CComponent.h"
#include "Client_Enum.h"

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
	_vec3 vRot[PP_END];
	void Reset() { ZeroMemory(&vRot, sizeof(_vec3) * PP_END ); }
};

struct TActionPose
{
	TActionPose() { Reset(); }
	_vec3 vRot[PP_END];
	bool bMask[PP_END];
	void Reset()
	{
		ZeroMemory(&vRot, sizeof(_vec3) * PP_END);
		ZeroMemory(&bMask, sizeof(bool) * PP_END);
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

	void SetBuffer(const array<TPlayerBuffer, PP_END>& tBuffer);
	void SetInitialTransform();

	void RenderDebugTransform();

	inline bool OnAction() { return m_eAction != EPlayerActionState::NONE; }

	constexpr static int PP_ROOT = -1;
	static constexpr float s_fForward = -1.f; /* LH 기준 팔 +x 회전 */
	static constexpr float s_fLocoBlendSpeed = 10.f; /* 로코모션이 바뀔 때 목표 Param까지 멤버 Param이 쫓아가는 속도 */
	static constexpr float s_fActionBlendSpeed = 20.f; /* 액션이 시작될/끝날 때 목표 Param까지 멤버 Param이 쫓아가는 속도 */

private:
	void UpdateLocomotion(float fTimeDelta);
	void UpdateAction(float fTimeDelta);
	void ApplyPose();
	static TLocoParam GetLocoParam(EPlayerLocomotionState eLoco);

	// Root
	// ├─ vBody
	// │  ├─ vHead
	// │  ├─ vLArm
	// │  └─ vRArm
	// ├─ vLLeg
	// └─ vRLeg

	/* 버퍼 저장용 */
	const static array<int, PP_END> s_arrParent;
	const static array<PLAYERPART, PP_END> s_arrUpdateOrder;
	static const char* s_szPartName[PP_END];
	array<TPlayerBuffer, PP_END> m_arrBuffer = {};

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
	void SampleStretchArms();
	static constexpr float s_fRipperDuration = 3.f;

	/* 애니메이션 디버그용 */
	bool m_bAnimPause = false;

public:
	static CPlayerAnimator* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};

