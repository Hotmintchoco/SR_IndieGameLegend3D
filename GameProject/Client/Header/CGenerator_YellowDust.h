#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CTransform;
}

class CGenerator_YellowDust : public CGameObject
{
protected:
	explicit CGenerator_YellowDust(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGenerator_YellowDust();

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
	_vec3 m_vEffectSpawnLocation = { 0.f, 0.f, 0.f };
	_bool m_bStart = false;
	_float m_fEffectCoolTime = 0.125f;
	_float m_fEffectCoolTime2 = 5.f;
	_float m_fElapsedTime = 0.f;
	_float m_fElapsedTime2 = m_fEffectCoolTime2;

	_vec3 m_vDir[4] = {
		{  4.f, -4.f,  4.f },
		{  4.f, -4.f, -4.f },
		{ -4.f, -4.f, -4.f },
		{ -4.f, -4.f,  4.f },
	};
	_int m_iIndex = 0;


public:
	static CGenerator_YellowDust* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

