#pragma once

#include "CGameObject.h"
#include "Client_Enum.h"
#include "Client_Struct.h"

namespace Engine
{
	class CTransform;
}

class CTile : public CGameObject
{
protected:
	explicit CTile(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTile(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx);
	virtual ~CTile();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	
	inline int GetPositionIndex() { return m_iPositionIdx; }
	inline TTileIdx GetPositionIndex2D() { return m_tPositionIdx2D; }
	inline ETileType GetType() { return m_eType; }
	inline void SetType(ETileType eType) { m_eType = eType; }

protected:
	HRESULT			Add_Component();

	Engine::CTransform* m_pTransformCom = nullptr;

	int m_iPositionIdx = -1;
	TTileIdx m_tPositionIdx2D = TTileIdx{ -1, -1 };

	ETileType m_eType = ETileType::NONE;

protected:
	virtual void		Free();
};

