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

	inline float GetSprintCoef() { return m_fSprintCoef; }

	void Knockback(const _vec3& vDir, float fIntensity);
	void Walk(const _vec2& vCommand);
	void Stop();

private:
	float m_fSprintCoef = 2.f;
	float m_fKnockbackAngle = D3DXToRadian(30.f);

public:
	static CPlayerMovement* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};

