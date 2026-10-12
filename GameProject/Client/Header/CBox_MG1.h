#pragma once
#include "CTile.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
	class CCollider;
}

class CBox_MG1 : public CTile
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

	/* IAnamorphicTarget */
	virtual CVIBuffer* GetBuffer() override;

private:
	HRESULT			Add_Component();

public:
	static CBox_MG1* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CBox_MG1* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos);
	static CBox_MG1* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _int iIndex);
	static CBox_MG1* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _int iIndex, _vec3 vDir);


	void Set_Pos(const _vec3& vPos);

	void Set_Connection(_bool bConnection) { m_bConnection = bConnection; }
	void Set_TextureIdx(_int bIdx) { m_iTextureIdx = bIdx; }
	void Set_Dir(_vec3 vDir) { m_vDir = vDir; }
protected:
	virtual void		Free();

protected:
	Engine::CRcTex* m_pBufferCom = nullptr;
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CTransform* m_pTransformCom2 = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CCalculator* m_pCalculatorCom = nullptr;
	Engine::CCollider* m_pColliderCom = nullptr;

	_bool m_bConnection = false;
	_int m_iTextureIdx = 0;

	_vec3 m_vDir = { 0.f,0.f,0.f };
};
