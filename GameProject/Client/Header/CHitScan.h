#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CTransform;
	class CTexture;
}

class CLaserBuffer;
class CMonster;

class CHitScan : public CGameObject
{
protected:
	explicit CHitScan(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, CMonster* pTarget);
	virtual ~CHitScan();

public:
	virtual	HRESULT Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

private:
	HRESULT	Add_Component();
	void CalculateLength();

	CLaserBuffer* m_pBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture* m_pTexture = nullptr;

	CMonster* m_pTarget = nullptr;

	float m_fLifeTime = 1.f;
	float m_fTimeAfterBirth = 0.f;
	int m_iOpacity = 100;
	_vec3 m_vStart{ 0.f, 0.f, 0.f };
	_vec3 m_vDir{ 0.f, 0.f, 0.f };
	float m_fLength = 0.f;
	float m_fWidth = 0.05f;

public:
	static CHitScan* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, CMonster* pTarget);

protected:
	virtual void Free() override;
};

