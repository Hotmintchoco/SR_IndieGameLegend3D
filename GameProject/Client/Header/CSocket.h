#pragma once

#include "CComponent.h"

namespace Engine
{
	class CTransform;
}

class CSocket : public CComponent
{
protected:
	explicit CSocket(LPDIRECT3DDEVICE9 pGraphicDev, CTransform* pParentTransform);
	virtual ~CSocket();
	virtual CComponent* Clone() { assert(0); return nullptr; };

public:
	virtual _int Update_Component(_float fTimeDelta) override;
	virtual void LateUpdate_Component() override;

	void UpdateSocket(); /* 업데이트 순서가 중요해서 기존 업데이트 로직에서 따로 뺌 */
	
	inline void SetTarget(CTransform* pTransform) { m_pTargetTransform = pTransform; }
	inline void SetOffset(const _matrix& mat) { m_matOffset = mat; }

protected:
	CTransform* m_pParentTransform = nullptr; /* 참조만 */
	CTransform* m_pTargetTransform = nullptr; /* 참조만 */

	_matrix m_matOffset;

public:
	static CSocket* Create(LPDIRECT3DDEVICE9 pGraphicDev, CTransform* pParentTransform);

protected:
	virtual void Free() override;
};

