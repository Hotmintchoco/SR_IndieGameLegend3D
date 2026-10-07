#pragma once

#include "CBase.h"
#include "Engine_Define.h"
#include "CPostEffect.h"

BEGIN(Engine)

class IRenderable;

class ENGINE_DLL CRenderer : public CBase
{
	DECLARE_SINGLETON(CRenderer)

private:
	explicit CRenderer();
	virtual ~CRenderer();

	struct PIPELINESTATE
	{
		DWORD dwFill = D3DFILL_SOLID;
		DWORD dwCull = D3DCULL_CCW;
		DWORD dwLighting = TRUE;
		DWORD dwColorOp = D3DTOP_MODULATE;
		DWORD dwColorArg1 = D3DTA_TEXTURE;
		DWORD dwAlphaOp = D3DTOP_SELECTARG1;
		DWORD dwAlphaArg1 = D3DTA_TEXTURE;
		DWORD dwTexFactor = 0xffffffff;
		LPDIRECT3DBASETEXTURE9 pTexture = nullptr;
	};

	void Begin_WireFrame(LPDIRECT3DDEVICE9& pGraphicDev, PIPELINESTATE& tOld, D3DCOLOR dwColor);
	void End_WireFrame(LPDIRECT3DDEVICE9& pGraphicDev, PIPELINESTATE& tOld);

public:
	void	Add_RenderGroup(RENDERID eID, IRenderable* pRenderable);
	void	Render(LPDIRECT3DDEVICE9& pGraphicDev);
	void	Clear_RenderGroup();

    // Ownership is transferred only when registration succeeds.
    _bool Register_PostEffect(POST_EFFECT eType, CPostEffect* pEffect);
    CPostEffect* Get_PostEffect(POST_EFFECT eType) const;
    _bool Set_PostEffect(POST_EFFECT eType);
    POST_EFFECT Get_PostEffectType() const { return m_ePostEffect; }
    void Update_PostEffect(_float fTimeDelta);
    _bool Begin_PostEffect(LPDIRECT3DDEVICE9 pDevice);
    void End_PostEffect(LPDIRECT3DDEVICE9 pDevice);
public:
	void	Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev);
	void	Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void	Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void	Render_AlphaTest(LPDIRECT3DDEVICE9& pGraphicDev);
	void	Render_UI(LPDIRECT3DDEVICE9& pGraphicDev);

	/* 디버그 용 */
	void Render_Collider(LPDIRECT3DDEVICE9& pGraphicDev);
	void Render_DebugTriangle(LPDIRECT3DDEVICE9& pGraphicDev);
	void Add_DebugTriangle(const std::array<_vec3, 3>& vTri, const _vec3& vNormal, D3DCOLOR dwColor = D3DCOLOR_ARGB(255, 255, 0, 0));

private:
	list<IRenderable*>		m_RenderGroup[RENDER_END];

    std::map<POST_EFFECT, CPostEffect*> m_postEffects;
    POST_EFFECT m_ePostEffect = POST_EFFECT::NONE;
    CPostEffect* m_pCapturedEffect = nullptr;
	struct TDebugTri
	{
		std::array<_vec3, 3> vTri;
		_vec3                vNormal;
		D3DCOLOR             dwColor;
	};
	std::vector<TDebugTri> m_vecDebugTri;


private:
	virtual void	Free();

};

END
