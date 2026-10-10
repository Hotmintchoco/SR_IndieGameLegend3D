#pragma once

#include "CProjectile.h"
#include "CEventDelegate.h"

namespace Engine
{
	class CTexture;
	class CSphereCollider;
}

class CRibbonBuffer;

class CRibbon : public CProjectile
{
protected:
	explicit CRibbon(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir);
	virtual ~CRibbon();

public:
	virtual	HRESULT Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;

	void StartExplosionPhase();

	CEventDelegate<void> m_OnExplosionPhaseEnded;

private:
	HRESULT	Add_Component();
	void InitializeWorldMatrix();
	void UpdateInput(float fTimeDelta);
	void UpdateCameraShot(float fTimeDelta);
	void UpdateBuffer();
	void UpdateExplosionPhase(float fTimeDelta);

	CRibbonBuffer* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CSphereCollider* m_pColliderCom = nullptr;

	inline static TProjectileData s_tData = []()->TProjectileData {
		TProjectileData t;
		t.fSpeed = 4.f;
		t.fLifeTime = 999.f;
		return t;
		}();

	_vec3 m_vPrevPos;
	float m_fTraveled = 0.f;
	float m_fTraveledSingleSegment = 0.f;
	float m_fSampleIntervalLength = 0.1f; /* 진행 거리마다 샘플링 */
	float m_fWidth = 0.2f;

	/* 카메라 설정 */
	_vec3 m_vOffset{ 0.f, 0.3f, -0.5f };
	float m_fCursorSensitivity = 0.1f;

	/* 폭발 연출 저장용 */
	vector<_vec3> m_vecExplodePoint;
	float m_fSampleExplosionLength = 1.f;
	float m_fExplodePropagationSpeed = 10.f;
	float m_fTraveledAfterExplodeMark = 0.f;
	bool m_bOnExplosionPhase = false;
	int m_iCurrentIndex = -1;
	int m_iIgnoreExplosionCount = 3; /* 시작부터 터지면 플레이어가 피격됨 */
	bool m_bCamFollowing = false; /* 처음부터 카메라 위치를 맞추기 위한 변수 */

public:
	static CRibbon* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir);

protected:
	virtual void Free() override;
};

