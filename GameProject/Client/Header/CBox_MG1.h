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

class CBox_MG1 : public CGameObject
{
protected:
	explicit CBox_MG1(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBox_MG1();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(COLLINFO eCollInfo) override;

private:
	HRESULT			Add_Component();

public:
	static CBox_MG1* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CBox_MG1* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos);


	void Set_Pos(const _vec3& vPos);
protected:
	virtual void		Free();

protected:
	Engine::CRcTex* m_pBufferCom = nullptr;
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CCalculator* m_pCalculatorCom = nullptr;
	Engine::CCollider* m_pColliderCom = nullptr;
};
