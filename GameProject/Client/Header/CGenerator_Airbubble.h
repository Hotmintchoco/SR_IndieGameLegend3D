#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CTransform;
}

class CGenerator_Airbubble : public CGameObject
{
protected:
	explicit CGenerator_Airbubble(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGenerator_Airbubble();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

	_float GetCenterX(_float x)
	{
		return 60.f + 15.f * floorf((x - 60.f + 7.5f) / 15.f);
	}

	_float GetCenterZ(_float z)
	{
		return 60.f + 13.f * floorf((z - 60.f + 6.5f) / 13.f);
	}
private:
	Engine::CTransform* m_pTransformCom = nullptr;
	
	_vec3 m_vRoomCenterLocation = { 0.f, 0.f, 0.f };
	_bool m_bStart = false;
	_float m_fElapsedTime = 0.f;
	_float m_fEffectCoolTime = 0.5f;


public:
	static CGenerator_Airbubble* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

