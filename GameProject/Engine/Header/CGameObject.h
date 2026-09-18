#pragma once

#include "CBase.h"
#include "CComponent.h"
#include "IRenderable.h"

BEGIN(Engine)

class CLayer;
class CCollider;

class ENGINE_DLL CGameObject : public CBase, public IRenderable
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject();

public:
	CComponent* Get_Component(COMPONENTID eID, const _tchar* pComponentTag);

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject()PURE;

	virtual			void		OnCollisionEnter(CGameObject* pOther) {}
	virtual			void		OnCollisionStay(CGameObject* pOther) {}
	virtual			void		OnCollisionExit(CGameObject* pOther) {}

	inline void					SetOwner(CLayer* pLayer) { m_pOwner = pLayer; }

	/* IRenderable */
	virtual void Render(LPDIRECT3DDEVICE9& pDevice) { Render_GameObject(); }
	virtual _float		Get_ViewZ() { return m_fViewZ; }
	virtual CBase* GetBase() { return static_cast<CBase*>(this); }
	/* ----------- */

public:
	void			Set_Dead(_bool bDead) { m_bDead = bDead; }
	_bool			Is_Dead() const { return m_bDead; }

	void			Set_IsActive(_bool bIsActive) { m_bIsActive = bIsActive; }
	_bool			Get_IsActive() const { return m_bIsActive; }

	void			Compute_ViewZ(const _vec3* pPos);

protected:
	void			Obstacle_Collision(CGameObject* pOther, CCollider* pObstacleCollider);

protected:
	map<const _tchar*, CComponent*>			m_mapComponent[ID_END];
	LPDIRECT3DDEVICE9						m_pGraphicDev;
	_float									m_fViewZ;
	_float 									m_fFrictionForce; // 마찰력 추가 (Speed에 곱해줌)
	_bool									m_bDead;
	_bool									m_bIsActive;	// 활성화 여부를 나타내는 변수	
	
	/* Ready 단계에서 Layer 접근이 불가한 문제를 해결하기 위한 변수로, Ready 단계 이후에는 보장되지 않음 */
	CLayer* m_pOwner = nullptr;

private:
	CComponent* Find_Component(COMPONENTID eID, const _tchar* pComponentTag);

protected:
	virtual		void		Free();

public:
	
};

END