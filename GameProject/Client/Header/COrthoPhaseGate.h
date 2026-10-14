#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CTransform;
	class CBoxCollider;
	class CTexture;
	class CRcTex;
	class CCamera;
}

/* 플레이어(3D)와 플랫 플레이어(2D)가 동시에 닿으면 방 레이어가 현재 페이즈를 클리어하는 게이트
   - 조건 변수는 COrthoRoomLayer가 보유
   - 이 객체는 충돌 Enter/Exit에서 해당 스위치만 켜고 끈다 */
class COrthoPhaseGate : public CGameObject
{
protected:
	explicit COrthoPhaseGate(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~COrthoPhaseGate();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;
	virtual void OnCollisionExit(COLLINFO eCollInfo) override;

	inline CTransform* GetTransform() { return m_pTransformCom; }

private:
	HRESULT Add_Component();
	void SetOwnerSwitch(_int iOtherID, bool bFlag);
	/* 원근 : 직교 카메라를 바라보도록 월드 행렬 재구성 (COrthoKey와 같은 방식) */
	void BillBoard(CCamera* pCamera);

	CTransform* m_pTransformCom = nullptr;
	CBoxCollider* m_pColliderCom = nullptr;

	CRcTex* m_pBufferCom = nullptr;
	CTexture* m_pTextureCom = nullptr;

	/* 스프라이트 애니메이션 */
	float m_fTimeAfterSpawn = 0.f;
	float m_fFrameInterval = 0.1f;
	int m_iCurrentTextureIndex = 0;
	int m_iTotalFrameCount = 0;

public:
	static COrthoPhaseGate* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};
