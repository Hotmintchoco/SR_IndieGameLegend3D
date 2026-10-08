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
	void Render_Collider(LPDIRECT3DDEVICE9& pGraphicDev);
	void Render_DebugTriangle(LPDIRECT3DDEVICE9& pGraphicDev);
	void Render_DebugScreen(LPDIRECT3DDEVICE9& pGraphicDev);
	void Add_DebugTriangle(const std::array<_vec3, 3>& vTri, const _vec3& vNormal, D3DCOLOR dwColor = D3DCOLOR_ARGB(255, 255, 0, 0));
	void Add_DebugScreenLine(const _vec2& vStart, const _vec2& vEnd, D3DCOLOR dwColor = D3DCOLOR_ARGB(255, 0, 255, 0));
	void Add_DebugScreenCross(const _vec2& vCenter, _float fSize = 10.f, D3DCOLOR dwColor = D3DCOLOR_ARGB(255, 0, 255, 0));
	void Add_DebugScreenRect(const _vec2& vCenter, _float fHalf = 10.f, D3DCOLOR dwColor = D3DCOLOR_ARGB(255, 0, 255, 0));
	void Add_DebugWorldMarker(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vWorldPos, _float fSize = 10.f, D3DCOLOR dwColor = D3DCOLOR_ARGB(255, 0, 255, 0));


private:
	list<IRenderable*>		m_RenderGroup[RENDER_END];

	struct TDebugTri
	{
		std::array<_vec3, 3> vTri;
		_vec3                vNormal;
		D3DCOLOR             dwColor;
	};
	std::vector<TDebugTri> m_vecDebugTri;
	struct VTXSCREEN
	{
		_vec4  vPos;     // x, y, z, rhw
		DWORD  dwColor;
	};
	static constexpr _ulong FVF_SCREEN = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
	std::vector<VTXSCREEN> m_vecDebugScreenLine;


private:
	virtual void	Free();

};

END
