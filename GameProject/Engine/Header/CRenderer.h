#pragma once

#include "CBase.h"
#include "Engine_Define.h"

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

public:
	void	Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev);
	void	Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void	Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void	Render_AlphaTest(LPDIRECT3DDEVICE9& pGraphicDev);
	void	Render_UI(LPDIRECT3DDEVICE9& pGraphicDev);

	/* 디버그 용 */
	void	Render_Collider(LPDIRECT3DDEVICE9& pGraphicDev);


private:
	list<IRenderable*>		m_RenderGroup[RENDER_END];

private:
	virtual void	Free();
};

END