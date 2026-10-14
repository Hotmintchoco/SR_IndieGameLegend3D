#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CTransform;
	class CBoxCollider;
	class CTexture;
	class CRcTex;
}

/* 페이즈 안의 열쇠
   - 플레이어 / 플랫 플레이어 중 누구든 닿으면 획득
   - 획득 시 방 레이어의 남은 키 카운트를 줄이고 사라진다 */
class COrthoKey : public CGameObject
{
protected:
	explicit COrthoKey(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~COrthoKey();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;

	inline CTransform* GetTransform() { return m_pTransform; }

private:
	HRESULT Add_Component();

	CRcTex* m_pBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CBoxCollider* m_pCollider = nullptr;
	CTexture* m_pTexture = nullptr; /* 임시 : 노란 텍스처 */

	/* 두 플레이어가 같은 프레임에 닿아도 한 번만 획득되도록 */
	bool m_bAcquired = false;

public:
	static COrthoKey* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};
