#pragma once

#include "CCubeNet.h"
#include "IVerticalTerrain.h"

namespace Engine
{
	class CTexture;
	class CTransform;
}

class CCubeNetPlank : public CCubeNet, public IVerticalTerrain
{
protected:
	explicit CCubeNetPlank(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCubeNetPlank();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;
	virtual void OnCollisionStay(COLLINFO eCollInfo) override;
	virtual void OnCollisionExit(COLLINFO eCollInfo) override;

	/* IVerticalTerrain */
	virtual bool GetFlatSolidBounds(_vec2& vMin, _vec2& vMax) override;

	/* 뚫린 바닥을 메웠을 때 방 레이어가 호출 : 두 콜라이더를 꺼서 다시 들 수 없게 한다 */
	void LockInHole();

private:
	HRESULT Add_Component();

	/* m_pColliderCom(넓은 범위)은 상호작용용 COLL_ROOMLOGIC, 이건 블럭 크기 장애물용 COLL_OBSTACLE */
	CBoxCollider* m_pObstacleColliderCom = nullptr;

public:
	static CCubeNetPlank* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};
