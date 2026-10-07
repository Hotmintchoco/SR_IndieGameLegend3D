#include "CPostEffect.h"
#include <fstream>

namespace
{
    template<typename T> void ReleaseResource(T*& p)
    {
        if (p) { p->Release(); p = nullptr; }
    }
}

CPostEffect::CPostEffect(const wchar_t* pShaderFile) : m_shaderFile(pShaderFile) {}
CPostEffect::~CPostEffect() { Release(); }

std::wstring CPostEffect::ResourcePath(const wchar_t* pFile)
{
    wchar_t exePath[MAX_PATH]{};
    const DWORD length = GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    if (!length || length >= MAX_PATH) return {};
    std::wstring path(exePath);
    return path.substr(0, path.find_last_of(L"\\/") + 1) + L"Resource\\Shader\\" + pFile;
}

HRESULT CPostEffect::Ready(LPDIRECT3DDEVICE9 pDevice, const D3DSURFACE_DESC& desc)
{
    if (!m_pShader)
    {
        const auto path = ResourcePath(m_shaderFile.c_str());
        std::ifstream file(path, std::ios::binary);
        if (!file)
        {
            OutputDebugStringW((L"Cannot read post effect: " + path + L"\n").c_str());
            return E_FAIL;
        }
        std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        // Editors may add a UTF-8 BOM, which the legacy HLSL compiler rejects.
        if (source.compare(0, 3, "\xEF\xBB\xBF") == 0) source.erase(0, 3);
        LPD3DXBUFFER code = nullptr, errors = nullptr;
        HRESULT hr = D3DXCompileShader(source.data(), static_cast<UINT>(source.size()),
            nullptr, nullptr, "main", "ps_2_0", 0, &code, &errors, nullptr);
        if (errors) OutputDebugStringA(static_cast<const char*>(errors->GetBufferPointer()));
        ReleaseResource(errors);
        if (SUCCEEDED(hr))
            hr = pDevice->CreatePixelShader(static_cast<const DWORD*>(code->GetBufferPointer()), &m_pShader);
        ReleaseResource(code);
        if (FAILED(hr)) return hr;
    }
    if (FAILED(Ready_Resources(pDevice))) return E_FAIL;
    D3DSURFACE_DESC current{};
    if (m_pSceneSurface) m_pSceneSurface->GetDesc(&current);
    if (current.Width != desc.Width || current.Height != desc.Height || current.Format != desc.Format)
    {
        ReleaseResource(m_pSceneSurface);
        ReleaseResource(m_pSceneTexture);
        HRESULT hr = pDevice->CreateTexture(desc.Width, desc.Height, 1, D3DUSAGE_RENDERTARGET,
            desc.Format, D3DPOOL_DEFAULT, &m_pSceneTexture, nullptr);
        if (FAILED(hr)) return hr;
        return m_pSceneTexture->GetSurfaceLevel(0, &m_pSceneSurface);
    }
    return S_OK;
}

_bool CPostEffect::Begin(LPDIRECT3DDEVICE9 pDevice)
{
	if (m_bFailed || m_pOutput) return false;
	if (FAILED(pDevice->TestCooperativeLevel()))
	{
		Release();
		return false;
	}
	LPDIRECT3DSURFACE9 output = nullptr;
	if (FAILED(pDevice->GetRenderTarget(0, &output))) return false;
	D3DSURFACE_DESC desc{};
	output->GetDesc(&desc);
	// The current device uses a non-MSAA depth buffer, shared by this target.
	if (desc.MultiSampleType != D3DMULTISAMPLE_NONE ||
		FAILED(Ready(pDevice, desc)))
	{
		OutputDebugStringW((L"Post effect unavailable: " + m_shaderFile + L". Using normal rendering.\n").c_str());
		m_bFailed = true;
		ReleaseResource(output);
		Release();
		return false;
	}
	pDevice->GetViewport(&m_viewport);
	if (FAILED(pDevice->SetRenderTarget(0, m_pSceneSurface)))
	{
		ReleaseResource(output);
		return false;
	}
	pDevice->SetViewport(&m_viewport);
	m_pOutput = output;
	return true;
}

void CPostEffect::End(LPDIRECT3DDEVICE9 pDevice)
{
	if (!m_pOutput) return;
	pDevice->SetRenderTarget(0, m_pOutput);
	pDevice->SetViewport(&m_viewport);
	LPDIRECT3DSTATEBLOCK9 state = nullptr;
	if (SUCCEEDED(pDevice->CreateStateBlock(D3DSBT_ALL, &state)))
	{
		D3DSURFACE_DESC desc{};
		m_pSceneSurface->GetDesc(&desc);
		const float w = static_cast<float>(desc.Width), h = static_cast<float>(desc.Height);
		struct SCREENVERTEX { float x, y, z, rhw, u, v; };
		// D3D9 half-pixel correction, matching texel centers to screen pixels.
		const SCREENVERTEX quad[] = {
			{ -0.5f, -0.5f, 0.f, 1.f, 0.f, 0.f },
			{ w - 0.5f, -0.5f, 0.f, 1.f, 1.f, 0.f },
			{ -0.5f, h - 0.5f, 0.f, 1.f, 0.f, 1.f },
			{ w - 0.5f, h - 0.5f, 0.f, 1.f, 1.f, 1.f } };
		pDevice->SetVertexShader(nullptr);
		pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
		pDevice->SetPixelShader(m_pShader);
		pDevice->SetTexture(0, m_pSceneTexture);
		
		for (DWORD sampler = 0; sampler < 16; ++sampler)
		{
            if (sampler > 0) pDevice->SetTexture(sampler, nullptr);
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
        const HRESULT bindResult = Bind_Resources(pDevice, desc);
		const HRESULT drawResult = SUCCEEDED(bindResult) ? pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(SCREENVERTEX)) : bindResult;
		state->Apply();
		ReleaseResource(state);
        if (FAILED(drawResult))
        {
            m_bFailed = true;
            pDevice->StretchRect(m_pSceneSurface, nullptr, m_pOutput, nullptr, D3DTEXF_NONE);
        }
	}
	else
	{
		// Preserve the rendered frame if state capture fails.
		pDevice->StretchRect(m_pSceneSurface, nullptr, m_pOutput, nullptr, D3DTEXF_NONE);
	}
	ReleaseResource(m_pOutput);
}

void CPostEffect::Release()
{
	ReleaseResource(m_pOutput);
	ReleaseResource(m_pSceneSurface);
	ReleaseResource(m_pSceneTexture);
	ReleaseResource(m_pShader);
	
}

