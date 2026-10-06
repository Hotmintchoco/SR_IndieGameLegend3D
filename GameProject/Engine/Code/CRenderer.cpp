#include "CRenderer.h"
#include "CDebugMgr.h"
#include "IRenderable.h"
#include "CGameObject.h"

namespace
{
	// 실제 객체는 모든 소유자가 참조를 반납하여 COM 참조 수가 0이 될 때 삭제된다
	template<typename T>
	void Release_PulseResource(T*& resource)
	{
		T* owned = resource;
		resource = nullptr;
		if (owned) owned->Release();
	}
}

IMPLEMENT_SINGLETON(CRenderer)

CRenderer::CRenderer()
{
}

CRenderer::~CRenderer()
{
	Free();
}

void CRenderer::Add_RenderGroup(RENDERID eID, IRenderable* pRenderable)
{
	if (eID >= RENDER_END || nullptr == pRenderable)
		return;

	m_RenderGroup[eID].push_back(pRenderable);
	/* 성철 : 기존 GameObject에서 Component 까지 확장하다 보니 CBase* 기반으로 변경 */
	// dynamic_cast<CBase*>(pRenderable)->AddRef();
	pRenderable->GetBase()->AddRef();
}

void CRenderer::Render(LPDIRECT3DDEVICE9& pGraphicDev)
{
	Render_Priority(pGraphicDev);
	Render_NonAlpha(pGraphicDev);
	Render_AlphaTest(pGraphicDev);
	Render_Alpha(pGraphicDev);
	Render_Collider(pGraphicDev);
	Render_DebugTriangle(pGraphicDev);

	Render_UI(pGraphicDev);

	Clear_RenderGroup();
}

void CRenderer::Clear_RenderGroup()
{
	for (size_t i = 0; i < RENDER_END; ++i)
	{
		/* 성철 : 결론적으로 그대로 둬도 되는거였는데, 어쩌다 보니 바꾸게 됨 */
		for (auto iter = m_RenderGroup[i].begin(); iter != m_RenderGroup[i].end(); ++iter)
		{
			//CBase* p = dynamic_cast<CBase*>(*iter);
			CBase* p = (*iter)->GetBase();
			Safe_Release<CBase*>(p);
		}
		m_RenderGroup[i].clear();
	}
}

void CRenderer::Begin_WireFrame(LPDIRECT3DDEVICE9& pGraphicDev, PIPELINESTATE& tOld, D3DCOLOR dwColor)
{
	pGraphicDev->GetRenderState(D3DRS_FILLMODE, &tOld.dwFill);
	pGraphicDev->GetRenderState(D3DRS_CULLMODE, &tOld.dwCull);
	pGraphicDev->GetRenderState(D3DRS_LIGHTING, &tOld.dwLighting);
	pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &tOld.dwTexFactor);
	pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &tOld.dwColorOp);
	pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &tOld.dwColorArg1);
	pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &tOld.dwAlphaOp);
	pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &tOld.dwAlphaArg1);
	pGraphicDev->GetTexture(0, &tOld.pTexture);   // AddRef 되므로 End에서 Release

	pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	/* 텍스쳐를 참조하지 않도록 컬러/알파 경로를 모두 TFACTOR로 고정 */
	pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwColor);
	pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
}

void CRenderer::End_WireFrame(LPDIRECT3DDEVICE9& pGraphicDev, PIPELINESTATE& tOld)
{
	pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, tOld.dwAlphaArg1);
	pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, tOld.dwAlphaOp);
	pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, tOld.dwColorArg1);
	pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, tOld.dwColorOp);
	pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, tOld.dwTexFactor);

	pGraphicDev->SetTexture(0, tOld.pTexture);
	if (nullptr != tOld.pTexture)
	{
		tOld.pTexture->Release();
		tOld.pTexture = nullptr;
	}

	pGraphicDev->SetRenderState(D3DRS_LIGHTING, tOld.dwLighting);
	pGraphicDev->SetRenderState(D3DRS_CULLMODE, tOld.dwCull);
	pGraphicDev->SetRenderState(D3DRS_FILLMODE, tOld.dwFill);
}

void CRenderer::Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& pObj : m_RenderGroup[RENDER_PRIORITY])
		pObj->Render(pGraphicDev);
}

void CRenderer::Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev)
{
	if (CDebugMgr::GetInstance()->GetMeshMode() == MESHRENDERMODE::MESH_NONE) return;

	_bool bWire = (CDebugMgr::GetInstance()->GetMeshMode() == MESHRENDERMODE::MESH_WIREFRAME);

	PIPELINESTATE tOld;
	if (bWire)
		Begin_WireFrame(pGraphicDev, tOld, D3DCOLOR_XRGB(200, 200, 200));

	for (auto& pObj : m_RenderGroup[RENDER_NONALPHA])
		pObj->Render(pGraphicDev);

	if (bWire)
		End_WireFrame(pGraphicDev, tOld);
}

void CRenderer::Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev)
{
	if (CDebugMgr::GetInstance()->GetMeshMode() == MESHRENDERMODE::MESH_NONE) return;

	_bool bWire = (CDebugMgr::GetInstance()->GetMeshMode() == MESHRENDERMODE::MESH_WIREFRAME);

	m_RenderGroup[RENDER_ALPHA].sort([](IRenderable* pDst, IRenderable* pSrc)->bool
		{
			return pDst->Get_ViewZ() > pSrc->Get_ViewZ();
		});

	PIPELINESTATE tOld;

	if (bWire)
	{
		Begin_WireFrame(pGraphicDev, tOld, D3DCOLOR_XRGB(200, 200, 200));
	}
	else
	{
		pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}

	for (auto& pObj : m_RenderGroup[RENDER_ALPHA])
		pObj->Render(pGraphicDev);

	if (bWire)
	{
		End_WireFrame(pGraphicDev, tOld);
	}
	else
	{
		pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
}

void CRenderer::Render_AlphaTest(LPDIRECT3DDEVICE9& pGraphicDev)
{
	if (CDebugMgr::GetInstance()->GetMeshMode() == MESHRENDERMODE::MESH_NONE) return;

	_bool bWire = (CDebugMgr::GetInstance()->GetMeshMode() == MESHRENDERMODE::MESH_WIREFRAME);

	PIPELINESTATE tOld;

	if (bWire)
	{
		Begin_WireFrame(pGraphicDev, tOld, D3DCOLOR_XRGB(200, 200, 200));
	}
	else
	{
		pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
		pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x80);

	}

	for (auto& pObj : m_RenderGroup[RENDER_ALPHATEST])
		pObj->Render(pGraphicDev);

	if (bWire)
	{
		End_WireFrame(pGraphicDev, tOld);
	}
	else
	{
		pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x00);
	}
}

void CRenderer::Render_UI(LPDIRECT3DDEVICE9& pGraphicDev)
{
	_matrix matOldView, matOldProj, matView, matProj;
	pGraphicDev->GetTransform(D3DTS_VIEW, &matOldView);
	pGraphicDev->GetTransform(D3DTS_PROJECTION, &matOldProj);

	D3DXMatrixIdentity(&matView);
	D3DXMatrixOrthoLH(&matProj, (float)WINCX, (float)WINCY, 0.f, 1.f);

	pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_RenderGroup[RENDER_UI].sort([](IRenderable* pDst, IRenderable* pSrc)
		{
			return pDst->Get_Z() > pSrc->Get_Z();
		});

	for (auto& pObj : m_RenderGroup[RENDER_UI])
		pObj->Render(pGraphicDev);

	pGraphicDev->SetTransform(D3DTS_VIEW, &matOldView);
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOldProj);
}

void CRenderer::Render_Collider(LPDIRECT3DDEVICE9& pGraphicDev)
{
	if (!CDebugMgr::GetInstance()->GetShowCollider()) return;

	auto& rGroup = m_RenderGroup[RENDER_DEBUG_COLLIDER];
	if (rGroup.empty())
		return;

	_matrix matOldWorld;
	pGraphicDev->GetTransform(D3DTS_WORLD, &matOldWorld);

	PIPELINESTATE tOld;
	Begin_WireFrame(pGraphicDev, tOld, D3DCOLOR_XRGB(0, 255, 0));

	for (auto& pObj : rGroup)
		pObj->Render(pGraphicDev);

	End_WireFrame(pGraphicDev, tOld);

	pGraphicDev->SetTransform(D3DTS_WORLD, &matOldWorld);
}

void CRenderer::Render_DebugTriangle(LPDIRECT3DDEVICE9& pGraphicDev)
{
	if (!CDebugMgr::GetInstance()->GetShowDebugTriangle()) return;

	if (m_vecDebugTri.empty())
		return;

	// 상태 백업
	DWORD dwLighting, dwCull;
	pGraphicDev->GetRenderState(D3DRS_LIGHTING, &dwLighting);
	pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
	_matrix matOldWorld, matIdentity;
	pGraphicDev->GetTransform(D3DTS_WORLD, &matOldWorld);
	D3DXMatrixIdentity(&matIdentity);

	pGraphicDev->SetTransform(D3DTS_WORLD, &matIdentity);
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pGraphicDev->SetTexture(0, nullptr);
	pGraphicDev->SetFVF(FVF_COL);

	// 요청된 삼각형들을 한 번에 모아서 드로우콜 1회
	std::vector<VTXCOL> vecVtx;
	vecVtx.reserve(m_vecDebugTri.size() * 3);
	for (const auto& t : m_vecDebugTri)
	{
		_vec3 vN;
		D3DXVec3Normalize(&vN, &t.vNormal);
		for (int i = 0; i < 3; ++i)
			vecVtx.push_back({ t.vTri[i] + vN * 0.002f, t.dwColor });
	}
	pGraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
		(UINT)m_vecDebugTri.size(), vecVtx.data(), sizeof(VTXCOL));

	// 복구
	pGraphicDev->SetTransform(D3DTS_WORLD, &matOldWorld);
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, dwLighting);
	pGraphicDev->SetRenderState(D3DRS_CULLMODE, dwCull);

	m_vecDebugTri.clear();   // 매 프레임 새로 요청받는 방식
}

void CRenderer::Add_DebugTriangle(const std::array<_vec3, 3>& vTri, const _vec3& vNormal, D3DCOLOR dwColor)
{
	m_vecDebugTri.push_back({ vTri, vNormal, dwColor });
}

void CRenderer::Update_PulseEffect(_float fTimeDelta)
{
	if (m_bPulseEnabled && fTimeDelta > 0.f)
	{
		m_fPulseTime = fmodf(m_fPulseTime + fTimeDelta * m_fPulseSpeed, D3DX_PI * 2.f);
		m_fWaterDropTime = fmodf(m_fWaterDropTime + fTimeDelta * m_fWaterDropSpeed, D3DX_PI * 2.f);
	}
}

void CRenderer::Set_PulseParameters(_float fStrength, _float fSpeed)
{
	m_fPulseAmplitude = max(0.f, min(fStrength, 0.25f));
	m_fPulseSpeed = max(0.f, fSpeed);
}

void CRenderer::Set_WaterDropParameters(_float fStrength, _float fSpeed)
{
	m_fWaterDropAmplitude = max(0.f, min(fStrength, 0.25f));
	m_fWaterDropSpeed = max(0.f, fSpeed);
}

HRESULT CRenderer::Ready_PulseEffect(LPDIRECT3DDEVICE9 pDevice, const D3DSURFACE_DESC& desc)
{
	if (!m_pPulseShader)
	{
        // Resolve resources beside the executable, independent of VS working directory.
        wchar_t exePath[MAX_PATH] = {};
        const DWORD length = GetModuleFileNameW(nullptr, exePath, MAX_PATH);
        if (!length || length >= MAX_PATH) return E_FAIL;
        std::wstring directory(exePath);
        directory = directory.substr(0, directory.find_last_of(L"\\/") + 1);
        const std::wstring shaderPath = directory + L"Resource\\Shader\\WaterDrop.hlsl";
        LPD3DXBUFFER code = nullptr;
        LPD3DXBUFFER errors = nullptr;
        HRESULT hr = D3DXCompileShaderFromFileW(shaderPath.c_str(), nullptr, nullptr,
            "main", "ps_2_0", 0, &code, &errors, nullptr);
		if (errors)
			OutputDebugStringA(static_cast<const char*>(errors->GetBufferPointer()));
		Release_PulseResource(errors);
		if (SUCCEEDED(hr))
			hr = pDevice->CreatePixelShader(static_cast<const DWORD*>(code->GetBufferPointer()), &m_pPulseShader);
		Release_PulseResource(code);
		if (FAILED(hr)) return hr;
	}

    if (!m_pWaterDropTexture)
    {
        wchar_t exePath[MAX_PATH] = {};
        const DWORD length = GetModuleFileNameW(nullptr, exePath, MAX_PATH);
        if (!length || length >= MAX_PATH) return E_FAIL;
        std::wstring directory(exePath);
        directory = directory.substr(0, directory.find_last_of(L"\\/") + 1);
        const std::wstring texturePath = directory + L"Resource\\Shader\\CameraFilterPack_RainDrop.png";
        HRESULT hr = D3DXCreateTextureFromFileExW(pDevice, texturePath.c_str(),
            D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 0, D3DFMT_A8R8G8B8,
            D3DPOOL_MANAGED, D3DX_FILTER_LINEAR, D3DX_FILTER_NONE, 0,
            nullptr, nullptr, &m_pWaterDropTexture);
        if (FAILED(hr)) return hr;
    }
	D3DSURFACE_DESC current{};
	if (m_pPulseSurface) m_pPulseSurface->GetDesc(&current);
	if (current.Width != desc.Width || current.Height != desc.Height || current.Format != desc.Format)
	{
		Release_PulseResource(m_pPulseSurface);
		Release_PulseResource(m_pPulseTexture);
		HRESULT hr = pDevice->CreateTexture(desc.Width, desc.Height, 1, D3DUSAGE_RENDERTARGET,
			desc.Format, D3DPOOL_DEFAULT, &m_pPulseTexture, nullptr);
		if (FAILED(hr)) return hr;
		return m_pPulseTexture->GetSurfaceLevel(0, &m_pPulseSurface);
	}
	return S_OK;
}

_bool CRenderer::Begin_PulseEffect(LPDIRECT3DDEVICE9 pDevice)
{
	if (!m_bPulseEnabled || m_bPulseFailed || m_pPulseOutput) return false;
	if (FAILED(pDevice->TestCooperativeLevel()))
	{
		Release_PulseEffect();
		return false;
	}
	LPDIRECT3DSURFACE9 output = nullptr;
	if (FAILED(pDevice->GetRenderTarget(0, &output))) return false;
	D3DSURFACE_DESC desc{};
	output->GetDesc(&desc);
	// The current device uses a non-MSAA depth buffer, shared by this target.
	if (desc.MultiSampleType != D3DMULTISAMPLE_NONE ||
		FAILED(Ready_PulseEffect(pDevice, desc)))
	{
		OutputDebugStringA("Water-drop effect unavailable; check Resource/Shader/WaterDrop.hlsl and CameraFilterPack_WaterDrop.png beside Client.exe. Using normal rendering.\n");
		m_bPulseFailed = true;
		Release_PulseResource(output);
		Release_PulseEffect();
		return false;
	}
	pDevice->GetViewport(&m_tPulseViewport);
	if (FAILED(pDevice->SetRenderTarget(0, m_pPulseSurface)))
	{
		Release_PulseResource(output);
		return false;
	}
	pDevice->SetViewport(&m_tPulseViewport);
	m_pPulseOutput = output;
	return true;
}

void CRenderer::End_PulseEffect(LPDIRECT3DDEVICE9 pDevice)
{
	if (!m_pPulseOutput) return;
	pDevice->SetRenderTarget(0, m_pPulseOutput);
	pDevice->SetViewport(&m_tPulseViewport);
	LPDIRECT3DSTATEBLOCK9 state = nullptr;
	if (SUCCEEDED(pDevice->CreateStateBlock(D3DSBT_ALL, &state)))
	{
		D3DSURFACE_DESC desc{};
		m_pPulseSurface->GetDesc(&desc);
		const float w = static_cast<float>(desc.Width), h = static_cast<float>(desc.Height);
		D3DSURFACE_DESC dropDesc{};
		m_pWaterDropTexture->GetLevelDesc(0, &dropDesc);
		const float constants[] = { m_fPulseAmplitude, sinf(m_fPulseTime), m_fWaterDropAmplitude, sinf(m_fWaterDropTime),
			0.5f / w, 0.5f / h, 1.f / dropDesc.Width, 1.f / dropDesc.Height };
		struct SCREENVERTEX { float x, y, z, rhw, u, v; };
		// D3D9 half-pixel correction, matching texel centers to screen pixels.
		const SCREENVERTEX quad[] = {
			{ -0.5f, -0.5f, 0.f, 1.f, 0.f, 0.f },
			{ w - 0.5f, -0.5f, 0.f, 1.f, 1.f, 0.f },
			{ -0.5f, h - 0.5f, 0.f, 1.f, 0.f, 1.f },
			{ w - 0.5f, h - 0.5f, 0.f, 1.f, 1.f, 1.f } };
		pDevice->SetVertexShader(nullptr);
		pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
		pDevice->SetPixelShader(m_pPulseShader);
		pDevice->SetPixelShaderConstantF(0, constants, 2);
		pDevice->SetTexture(0, m_pPulseTexture);
		pDevice->SetTexture(1, m_pWaterDropTexture);
		for (DWORD sampler = 0; sampler < 2; ++sampler)
		{
			pDevice->SetSamplerState(sampler, D3DSAMP_SRGBTEXTURE, FALSE);
			pDevice->SetSamplerState(sampler, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			pDevice->SetSamplerState(sampler, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			pDevice->SetSamplerState(sampler, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			pDevice->SetSamplerState(sampler, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			pDevice->SetSamplerState(sampler, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		}
		pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xf);
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(SCREENVERTEX));
		state->Apply();
		Release_PulseResource(state);
	}
	else
	{
		// Preserve the rendered frame if state capture fails.
		pDevice->StretchRect(m_pPulseSurface, nullptr, m_pPulseOutput, nullptr, D3DTEXF_NONE);
	}
	Release_PulseResource(m_pPulseOutput);
}

void CRenderer::Release_PulseEffect()
{
	Release_PulseResource(m_pPulseOutput);
	Release_PulseResource(m_pPulseSurface);
	Release_PulseResource(m_pPulseTexture);
	Release_PulseResource(m_pPulseShader);
	Release_PulseResource(m_pWaterDropTexture);
}

void CRenderer::Free()
{
	Release_PulseEffect();
	Clear_RenderGroup();
}
