#include "pch.h"
#include "CLavaEffect.h"

CLavaEffect::CLavaEffect() : CPostEffect(L"Lava.hlsl") {}

CLavaEffect::~CLavaEffect()
{
    if (m_pHeatTexture) m_pHeatTexture->Release();
}

void CLavaEffect::Update(_float fTimeDelta)
{
    if (!std::isfinite(fTimeDelta) || fTimeDelta <= 0.f) return;
    m_fTime = fmodf(m_fTime + fTimeDelta * m_fSpeed, D3DX_PI * 2.f);
}

void CLavaEffect::Set_HeatParameters(_float fStrength, _float fSpeed)
{
    if (!std::isfinite(fStrength) || !std::isfinite(fSpeed)) return;
    m_fStrength = max(0.f, min(fStrength, 0.08f));
    m_fSpeed = max(0.f, fSpeed);
}

void CLavaEffect::Set_TintStrength(_float fStrength)
{
    if (std::isfinite(fStrength)) m_fTintStrength = max(0.f, min(fStrength, 1.f));
}

HRESULT CLavaEffect::Ready_Resources(LPDIRECT3DDEVICE9 pDevice)
{
    if (m_pHeatTexture) return S_OK;
    const auto path = ResourcePath(L"CameraFilterPack_WaterDrop.png");
    if (path.empty()) return E_FAIL;
    const HRESULT hr = D3DXCreateTextureFromFileExW(pDevice, path.c_str(),
        D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 0, D3DFMT_A8R8G8B8,
        D3DPOOL_MANAGED, D3DX_FILTER_LINEAR, D3DX_FILTER_NONE, 0,
        nullptr, nullptr, &m_pHeatTexture);
    if (FAILED(hr)) OutputDebugStringW((L"Lava texture failed: " + path + L"\n").c_str());
    return hr;
}

HRESULT CLavaEffect::Bind_Resources(LPDIRECT3DDEVICE9 pDevice, const D3DSURFACE_DESC& desc)
{
    D3DSURFACE_DESC heatDesc{};
    if (FAILED(m_pHeatTexture->GetLevelDesc(0, &heatDesc))) return E_FAIL;
    const float constants[] = {
        m_fStrength, sinf(m_fTime), cosf(m_fTime), m_fTintStrength,
        0.5f / desc.Width, 0.5f / desc.Height, 1.f / heatDesc.Width, 1.f / heatDesc.Height };
    if (FAILED(pDevice->SetPixelShaderConstantF(0, constants, 2))) return E_FAIL;
    return pDevice->SetTexture(1, m_pHeatTexture);
}
