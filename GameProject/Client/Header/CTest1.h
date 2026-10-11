#pragma once
#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
	class CCollider;
}

class CTest1 : public CGameObject
{
protected:
	explicit CTest1(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTest1();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(COLLINFO eCollInfo) override;

private:
	HRESULT			Add_Component();

public:
	static CTest1* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CTest1* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iIdx);
	static CTest1* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iIdx, _vec3 vScale, _vec3 vPos);
	void Set_Idx(_int iIdx) { m_iIdx = iIdx; m_fFrame = m_iIdx; }
	void Set_Pos(_vec3 vPos);
	void Set_Scale(_vec3 vScale);
protected:
	virtual void		Free();

protected:
	Engine::CRcTex* m_pBufferCom = nullptr;
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CCalculator* m_pCalculatorCom = nullptr;
	Engine::CCollider* m_pColliderCom = nullptr;

	_int m_iIdx = 0;
	_float m_fFrame = 0.f;
};
