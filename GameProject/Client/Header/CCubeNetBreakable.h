#pragma once

#include "CCubeNet.h"
#include "IVerticalTerrain.h"
#include "CEventDelegate.h"

class CCubeNetBreakable : public CCubeNet, public IVerticalTerrain
{
protected:
	explicit CCubeNetBreakable(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCubeNetBreakable();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;
	virtual void OnCollisionStay(COLLINFO eCollInfo) override;

	/* IVerticalTerrain */
	virtual float SampleTerrainHeight(const _vec3& vRayStart) override;

	void SetHead(CCubeNetBreakable* pHead);

	CEventDelegate<CCubeNetBreakable*> m_OnDead;

private:
	HRESULT Add_Component();
	void PropagateDead();

	/* 헤드에게 전파 */
	CCubeNetBreakable* m_pHead = nullptr;
	vector<CCubeNetBreakable*> m_vecLinkedBreakable;

	/* 흔들림 - 사망 애니메이션 */
	bool m_bPendingDead = false;
	float m_fTimeAfterAnimation = 0.f;
	float m_fAnimationTime = 2.f;
	float m_fWiggleMagnitude = 0.3f;
	float m_fWiggleFrequency = 40.0f;
	int m_iSeedA = 0;
	int m_iSeedB = 0;
	int m_iSeedC = 0;

public:
	static CCubeNetBreakable* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};