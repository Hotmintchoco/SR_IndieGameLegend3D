#pragma once

#include "CMovement.h"

class IVerticalTerrain;
class COrthoRoomLayer;

class CFlatPlayerMovement : public CMovement
{
protected:
	explicit CFlatPlayerMovement(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFlatPlayerMovement();
	virtual CComponent* Clone() { assert(0); return nullptr; };

public:
	virtual _int Update_Component(_float fTimeDelta) override;
	virtual void LateUpdate_Component() override;

	void Walk(const float fCommand);
	void Jump();

	inline bool IsOnGround() { return m_bOnGround; }
	/* 순간이동(페이즈 시작/리셋) 시 이전 속도가 남지 않게 */
	inline void ResetVelocity() { m_vVelocity = { 0.f, 0.f, 0.f }; m_vImpulse = { 0.f, 0.f, 0.f }; m_bOnGround = false; }

protected:
	virtual void PerformMovement(float fTimeDelta) override;
	virtual void TryExertGravity(float fTimeDelta) override;
	virtual void ExertFriction(float fTimeDelta) override;
	virtual void ClampVelocity() override;
	/* 블럭 박스 기준 : x 이동 → 옆면 해소, z 이동 → 착지 / 머리 해소 */
	void TerrainResolver(const vector<IVerticalTerrain*>& vecTerrain, const _vec3& vPrev, _vec3& vDesired);
	_vec2 GetFlatSize();
	/* 방 안쪽 영역 밖으로 못 나가게 제한 (아래 끝은 바닥처럼 착지) */
	void RoomBoundsResolver(COrthoRoomLayer* pLayer, _vec3& vDesired);

	float m_fJumpSpeed = 5.f;

public:
	static CFlatPlayerMovement* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};

