#pragma once

#include "CMovement.h"

class CPlayerMovement : public CMovement
{
protected:
	explicit CPlayerMovement(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayerMovement();
	virtual CComponent* Clone() { assert(0); return nullptr; };

public:
	virtual _int Update_Component(_float fTimeDelta) override;
	virtual void LateUpdate_Component() override;

	inline void SetSprint(bool bFlag) { m_bSprinting = bFlag; }
	inline bool GetSprint() { return m_bSprinting; }

	void SetSpeedScale(float fScale);

	// 정민 : 템플런 용
	void SetJumpSpeed(float fSpeed) { m_fJumpSpeed = fSpeed; m_fJumpSpeedRaw = fSpeed; }

	void Knockback(const _vec3& vDir, float fIntensity);
	void Walk(const _vec2& vCommand);
	void Stop();
	void Jump();
	void Slip(float fTime);

	inline _vec3 GetVelocity() { return m_vVelocity; }
	inline void SetVelocity(const _vec3& vVel) { m_vVelocity = vVel; }

private:
	virtual float GetCurMaxGroundSpeed() const override;

	bool m_bSprinting = false;
	float m_fSprintCoef = 2.f;
	float m_fKnockbackAngle = D3DXToRadian(30.f);
	float m_fJumpSpeed = 3.5f;

	/* 가짜 스케일 관련 */
	float m_fSpeedScale = 1.f;
	float m_fMaxGroundSpeedRaw = 5.f;
	float m_fMaxAirSpeedHRaw = 5.f;
	float m_fMaxAirSpeedVRaw = 10.f;
	float m_fJumpSpeedRaw = 3.5f;
	float m_fGravityRaw = 9.8f;
	float m_fSkinRaw = 0.03f;

	/* 얼음판 미끄러짐 */
	float m_fLeftSlipTime = 0.f;
	bool m_bOnSlipping = false;

public:
	static CPlayerMovement* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};

