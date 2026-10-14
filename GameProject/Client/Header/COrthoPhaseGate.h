#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CTransform;
	class CBoxCollider;
	class CTexture;
	class CPlyTex;
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

	CTransform* m_pTransformCom = nullptr;
	CBoxCollider* m_pColliderCom = nullptr;

	/* 임시 : 큐브넷 버퍼 + 파란 텍스처 */
	CPlyTex* m_pBufferCom = nullptr;
	CTexture* m_pTextureCom = nullptr;

public:
	static COrthoPhaseGate* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};
