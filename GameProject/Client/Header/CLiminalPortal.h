#pragma once

#include "CLiminalObject.h"
#include <set>

class CLiminalPortal : public CLiminalObject
{
protected:
	explicit CLiminalPortal(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLiminalPortal();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;
	virtual void OnCollisionStay(COLLINFO eCollInfo) override;
	virtual void OnCollisionExit(COLLINFO eCollInfo) override;

	/* IRayTestable */
	virtual vector<pair<Engine::CVIBuffer*, Engine::CTransform*>> GetRayTestTargetInfo() override;

	float GetScale() const { return m_fScale; }
	CTransform* GetTransform() { return m_pTransformCom; }
	inline void SetLinkedPortal(CLiminalPortal* pPortal) { m_pLinkedPortal = pPortal; }
	inline void EnableGuard() { m_bTeleportGuard = true; m_fLeftGuartTime = m_fGuardTime; }

private:
	HRESULT Add_Component();
	Engine::CBoxCollider* Add_BoxCollider(const _tchar* pComponentKey, const _vec3& vExtents, const _vec3& vDiffPos);
	void TeleportTarget(CGameObject * pTarget, const _matrix& matTargetWorld);
	void CheckCrossCondition();

	/* 프레임 쪽 버퍼와 텍스쳐는 부모 클래스에 이미 선언, 단순 포탈 애니메이션 용 */
	CPlyTex* m_pAnimBuffer = nullptr;
	CTexture* m_pAnimTexture = nullptr;

	/* 콜라이더 (부모의 콜라이더는 사용하지 않고 nullptr 유지) */
	Engine::CBoxCollider* m_pFrameColliderL = nullptr;
	Engine::CBoxCollider* m_pFrameColliderR = nullptr;
	Engine::CBoxCollider* m_pAnimCollider = nullptr;

	/* 애니메이션 관련 */
	int m_iAnimFrameCount = -1;
	int m_iCurrentAnimFrame = 0;
	float m_fAnimFrameInterval = 0.1f;
	float m_fAnimAccTime = 0.f;

	CLiminalPortal* m_pLinkedPortal = nullptr;
	
	/* 통과하는 물체에 대해 상대방과의 스케일을 비교하여 그만큼 줄이거나 키워서 보냄 */
	float m_fScale = 1.f;

	/* 바로 텔레포트 처리되는 거 가드용 */
	float m_fGuardTime = 1.f;
	float m_fLeftGuartTime = 0.f;
	bool m_bTeleportGuard = false;

	/* 플레이어 이전 상태 캐싱 */
	_vec3 m_vPrevPos; /* 포탈 로컬 좌표 기준 */

public:
	static CLiminalPortal* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};

