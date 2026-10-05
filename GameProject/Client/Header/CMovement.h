#pragma once

#include "CComponent.h"

namespace Engine
{
	class CTransform;
}

struct TLaunchRequest
{
	_vec3 vVelocity{ 0.f, 0.f, 0.f };
	bool bOverrideH;
	bool bOverrideV;
	float fAlpha = 0.f;
};

class ITerrain;

class CMovement : public CComponent
{
protected:
	explicit CMovement(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMovement();
	virtual CComponent* Clone() { assert(0); return nullptr; };

public:
	virtual _int Update_Component(_float fTimeDelta) override;
	virtual void LateUpdate_Component() override;

	void AttachTransform(CTransform* pTransform);

protected:
	void PerformMovement(float fTimeDelta);
	void TryExertGravity(float fTimeDelta);
	void ExertFriction(float fTimeDelta);
	void AddImpulse(const _vec3& vDir, float fMagnitude);
	void ClampVelocity();
	void ClearFrameVariables();
	void Launch(const TLaunchRequest& tReq);
	virtual float GetCurMaxGroundSpeed() const { return m_fMaxGroundSpeed; }
	void TerrainResolver(const vector<ITerrain*>& vecTerrain, _vec3& vDesired);

	/* 부모 객체의 트랜스폼 */
	CTransform* m_pTransform = nullptr;

	_vec3 m_vVelocity{ 0.f, 0.f, 0.f };
	
	/* 특성 */
	float m_fInputAcc = 100.f;
	float m_fMaxGroundSpeed = 5.f;
	float m_fMaxAirSpeedH = 5.f;
	float m_fMaxAirSpeedV = 10.f;
	float m_fGroundFriction = 30.f;
	float m_fAirFriction = 0.1f;
	float m_fAirInputCoef = 0.2f;

	/* 공중 처리 */
	bool m_bOnGround = true;
	bool m_bPendingLaunch = false;

	/* 키 입력 기반 가속 커맨드 */
	_vec3 m_vAccCommand{ 0.f, 0.f, 0.f };

	/* 임펄스 */
	_vec3 m_vImpulse{ 0.f, 0.f, 0.f };

	/* Launch */
	TLaunchRequest m_tLaunchRequest;

	static constexpr float s_fGravity = 9.8f;

public:
	static CMovement* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};

