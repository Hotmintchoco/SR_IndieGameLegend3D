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

	// ------------ 쉐이더 왜곡 효과 ------------ 
	void	Set_PulseEnabled(_bool bEnabled) { m_bPulseEnabled = bEnabled; }
	_bool	Get_PulseEnabled() const { return m_bPulseEnabled; }
	void	Update_PulseEffect(_float fTimeDelta);
	//		확대/축소 강도, 반복 속도
	void	Set_PulseParameters(_float fStrength, _float fSpeed);
	_bool	Begin_PulseEffect(LPDIRECT3DDEVICE9 pDevice);
	void	End_PulseEffect(LPDIRECT3DDEVICE9 pDevice);
	// ------------ 쉐이더 왜곡 효과 ------------ 

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

	// 정민 : 쉐이더 왜곡 효과 용도
	_bool m_bPulseEnabled = false;
	_bool m_bPulseFailed = false;
	_float m_fPulseTime = 0.f;
	_float m_fPulseAmplitude = 0.12f;
	_float m_fPulseSpeed = 1.5f;
	LPDIRECT3DTEXTURE9 m_pPulseTexture = nullptr;
	LPDIRECT3DSURFACE9 m_pPulseSurface = nullptr;
	LPDIRECT3DSURFACE9 m_pPulseOutput = nullptr;
	LPDIRECT3DPIXELSHADER9 m_pPulseShader = nullptr;
	D3DVIEWPORT9 m_tPulseViewport{};

	struct TDebugTri
	{
		std::array<_vec3, 3> vTri;
		_vec3                vNormal;
		D3DCOLOR             dwColor;
	};
	std::vector<TDebugTri> m_vecDebugTri;


private:
	virtual void	Free();

	// 정민 : 쉐이더 왜곡 효과 용도
	HRESULT Ready_PulseEffect(LPDIRECT3DDEVICE9 pDevice, const D3DSURFACE_DESC& desc);
	void Release_PulseEffect();
};

END
