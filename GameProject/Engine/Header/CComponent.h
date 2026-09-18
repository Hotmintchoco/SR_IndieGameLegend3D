#pragma once

#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CComponent : public CBase
{
protected:
	explicit CComponent();
	explicit CComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent();

public:
	CGameObject*	Get_Owner() { return m_pOwner; }
	void			Set_Owner(CGameObject* pOwner) { m_pOwner = pOwner; }

	void			Set_IsActive(const bool& bIsActive) { m_bIsActive = bIsActive; }
	_bool			Get_IsActive() { return m_bIsActive; }

public:
	virtual _int Update_Component(const _float& fTimeDelta) { return 0; }
	virtual void LateUpdate_Component() {}

protected:
	LPDIRECT3DDEVICE9			m_pGraphicDev;
	_bool						m_bClone;
	_bool						m_bIsActive;	// 활성화 여부를 나타내는 변수

	CGameObject*				m_pOwner;

public:
	virtual CComponent* Clone()	PURE;

protected:
	virtual void		Free();
};

END