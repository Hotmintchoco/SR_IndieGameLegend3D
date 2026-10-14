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

protected:
	virtual void PerformMovement(float fTimeDelta) override;
	virtual void TryExertGravity(float fTimeDelta) override;
	virtual void ExertFriction(float fTimeDelta) override;
	virtual void ClampVelocity() override;
	void TerrainResolver(const vector<IVerticalTerrain*>& vecTerrain, _vec3& vDesired);
	/* 방 안쪽 영역 밖으로 못 나가게 제한 (아래 끝은 바닥처럼 착지) */
	void RoomBoundsResolver(COrthoRoomLayer* pLayer, _vec3& vDesired);

	float m_fJumpSpeed = 5.f;

public:
	static CFlatPlayerMovement* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};

