#pragma once

#include "CGameObject.h"
#include "IRayTestable.h"
#include "ITerrain.h"

namespace Engine
{
	class CTransform;
	class CTexture;
	class CPlyTex;
}

class CCube : public CGameObject, public IRayTestable, public ITerrain
{
protected:
	explicit CCube(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pTextureProtoTag);
	virtual ~CCube();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	/* IRayTestable */
	virtual vector<pair<Engine::CVIBuffer*, Engine::CTransform*>> GetRayTestTargetInfo() override;

	/* ITerrain */
	virtual float SampleTerrainHeight(const _vec3& vRayStart) override;

	inline Engine::CTransform* GetTransform() { return m_pTransformCom; }
	inline void SetTextureIndex(int iIdx) { m_iTextureIdx = iIdx; }
	inline int GetTextureIndex() { return m_iTextureIdx; }

	virtual void OnCollisionStay(COLLINFO eCollInfo) override;

private:
	HRESULT Add_Component();

	Engine::CPlyTex* m_pBufferCom = nullptr;
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CCollider* m_pColliderCom = nullptr;

	const _tchar* m_pTextureProtoTag = nullptr;
	int m_iTextureIdx = 0;

public:
	static CCube* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pTextureProtoTag);

private:
	virtual void		Free();
};