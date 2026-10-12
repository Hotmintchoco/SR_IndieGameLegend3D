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

class COpenCubeTex;

class COpenCube : public CGameObject, public IRayTestable, public ITerrain
{
protected:
	explicit COpenCube(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pTextureProtoTag);
	virtual ~COpenCube();

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

private:
	HRESULT Add_Component();

	COpenCubeTex* m_pBufferCom = nullptr;
	Engine::CPlyTex* m_pRayBufferCom = nullptr;
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;

	const _tchar* m_pTextureProtoTag = nullptr;
	int m_iTextureIdx = 0;

public:
	static COpenCube* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pTextureProtoTag);

private:
	virtual void		Free();
};