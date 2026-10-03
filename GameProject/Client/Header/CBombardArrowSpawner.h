#pragma once

#include "CGameObject.h"

class CBombardArrowSpawner : public CGameObject
{
protected:
	explicit CBombardArrowSpawner(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBombardArrowSpawner();

public:
	virtual	HRESULT Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override {}

private:
	void SpawnArrows(float fTimeDelta);

	float m_fTimeAfterBirth = 0.f;
	float m_fLifeTime = 1.0f;

	int m_iSpawnPerSecond = 3000;
	float m_fSpawnDist = 25.f;
	float m_fTargetVariance = 0.6f; /* 중심 기준 포격 영역 비율 */
	float m_fMinAngle = D3DXToRadian(75.f);
	float m_fMaxAngle = D3DXToRadian(80.f);
	float m_fMinShotPower = 0.9f;
	float m_fMaxShotPower = 1.f;

public:
	static CBombardArrowSpawner* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;

};


