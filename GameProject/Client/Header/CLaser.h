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
	float fWidth = 0.3f;
};

class CLaserBuffer;

class CLaser : public CProjectile
{
protected:
	explicit CLaser(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir);
	virtual ~CLaser();

public:
	virtual	HRESULT Ready_GameObject();
	virtual	_int Update_GameObject(const _float& fTimeDelta);
	virtual	void LateUpdate_GameObject(const _float& fTimeDelta);
	virtual	void Render_GameObject();

	virtual void OnCollisionEnter(CGameObject* pObject) override;

	static void RenderEditorPanel();

private:
	HRESULT	Add_Component();
	void BillBoardRoll();
	bool ComputeFacingUp(const _vec3& vCamPos, const _vec3& vPos, const _vec3& vDir, _vec3* pOutUp = nullptr);

	CLaserBuffer* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CSphereCollider* m_pColliderCom = nullptr;

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

public:
	static CLaser* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir);

protected:
	virtual void Free() override;
};


