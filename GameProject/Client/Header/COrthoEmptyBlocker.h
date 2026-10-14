#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CTransform;
	class CBoxCollider;
}

/* 바닥이 뚫린 칸
   - 버퍼/텍스처 없이 블럭 한 칸 크기 콜라이더만 있다
   - 3D 플레이어만 막고, 플랫 플레이어와는 충돌 검사를 하지 않는다 (COLL_OBSTACLE)
   - 같은 칸의 타일은 방 레이어가 비활성화해서 빈 공간처럼 보이게 한다 */
class COrthoEmptyBlocker : public CGameObject
{
protected:
	explicit COrthoEmptyBlocker(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~COrthoEmptyBlocker();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	virtual void OnCollisionStay(COLLINFO eCollInfo) override;

	inline CTransform* GetTransform() { return m_pTransform; }

private:
	HRESULT Add_Component();

	CTransform* m_pTransform = nullptr;
	CBoxCollider* m_pCollider = nullptr;

public:
	static COrthoEmptyBlocker* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};
