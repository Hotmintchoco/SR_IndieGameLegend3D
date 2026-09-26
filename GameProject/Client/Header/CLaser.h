#pragma once

#include "CProjectile.h"

namespace Engine
{
	class CTexture;
	class CSphereCollider;
}

struct TLaserData : public TProjectileData
{
	float fMaxLength = 5.f;
	int iRefelctionClone = 2;
	int iMaxReflection = 4;
	float fWidth = 0.3f;
};

class CLaserBuffer;

class CLaser : public CProjectile
{
protected:
	explicit CLaser(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir);
	explicit CLaser(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir, const float fLeftLifeTime, CGameObject* pIgnoreCollision);
	virtual ~CLaser();

public:
	virtual	HRESULT Ready_GameObject();
	virtual	_int Update_GameObject(const _float& fTimeDelta);
	virtual	void LateUpdate_GameObject(const _float& fTimeDelta);
	virtual	void Render_GameObject();

	virtual void OnCollisionEnter(CGameObject* pObject) override;

	void Reflect(const _vec3& vNormal);

	static void RenderEditorPanel();

private:
	HRESULT	Add_Component();
	void BillBoardRoll();
	void CalculateLength(const _float& fTimeDelta);
	bool ComputeFacingUp(const _vec3& vCamPos, const _vec3& vPos, const _vec3& vDir, _vec3* pOutUp = nullptr);

	CLaserBuffer* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CSphereCollider* m_pColliderCom = nullptr;
	Engine::CSphereCollider* m_pColliderComReflection = nullptr;

	/* 초기값 */
	_vec3 m_vStart{ 0.f, 0.f, 0.f };
	_vec3 m_vDir{ 0.f, 0.f, 0.f };

	inline static TLaserData s_tData = []()->TLaserData {
		TLaserData t;
		t.fLifeTime = 5.f;
		return t;
	}();

	float m_fCurrentLength = 0.f;
	bool m_bCollided = false;
	bool m_bShrinking = false;
	CGameObject* m_pPrevGenerationCollidedObject = nullptr; // 만들어지자마자 부딪히는 경우 방지
	float m_fReflectLength = 0.f; // 충돌 시점 길이
	float m_fReflectTime = 0.f; // 충돌 시점
	float m_fBirthTime = 0.f;
	bool m_bReflected = false; // 반사 이후에는 시각적 요소만 살려둠
	int m_iLeftReflection = s_tData.iMaxReflection;

public:
	static CLaser* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir);
	static CLaser* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir, const float fTimeAfterBirth, CGameObject* pIgnoreCollision);

protected:
	virtual void Free() override;
};


