#include "pch.h"
#include "CUnderwaterEffect.h"

using namespace Engine;

CUnderwaterEffect::CUnderwaterEffect() : CPostEffect(L"WaterDrop.hlsl") {}
CUnderwaterEffect::~CUnderwaterEffect()
{
    if (m_pWaterDropTexture) m_pWaterDropTexture->Release();
}

void CUnderwaterEffect::Update(_float fTimeDelta)
{
    if (!std::isfinite(fTimeDelta) || fTimeDelta <= 0.f) return;
    m_fPulseTime = fmodf(m_fPulseTime + fTimeDelta * m_fPulseSpeed, D3DX_PI * 2.f);
    m_fWaterDropTime = fmodf(m_fWaterDropTime + fTimeDelta * m_fWaterDropSpeed, D3DX_PI * 2.f);
}

void CUnderwaterEffect::Set_PulseParameters(_float fStrength, _float fSpeed)
{
    m_fPulseAmplitude = max(0.f, min(fStrength, 0.25f));
    m_fPulseSpeed = max(0.f, fSpeed);
}

void CUnderwaterEffect::Set_WaterDropParameters(_float fStrength, _float fSpeed)
{
    m_fWaterDropAmplitude = max(0.f, min(fStrength, 0.25f));
    m_fWaterDropSpeed = max(0.f, fSpeed);
}

HRESULT CUnderwaterEffect::Ready_Resources(LPDIRECT3DDEVICE9 pDevice)
{
    if (m_pWaterDropTexture) return S_OK;
    const auto path = ResourcePath(L"CameraFilterPack_RainDrop.png");
    if (path.empty()) return E_FAIL;
    const HRESULT hr = D3DXCreateTextureFromFileExW(pDevice, path.c_str(),
        D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 0, D3DFMT_A8R8G8B8,
        D3DPOOL_MANAGED, D3DX_FILTER_LINEAR, D3DX_FILTER_NONE, 0,
        nullptr, nullptr, &m_pWaterDropTexture);
    if (FAILED(hr)) OutputDebugStringW((L"Post effect texture failed: " + path + L"\n").c_str());
    return hr;
}

HRESULT CUnderwaterEffect::Bind_Resources(LPDIRECT3DDEVICE9 pDevice, const D3DSURFACE_DESC& desc)
{
    D3DSURFACE_DESC dropDesc{};
    if (FAILED(m_pWaterDropTexture->GetLevelDesc(0, &dropDesc))) return E_FAIL;
    const float constants[] = {
        m_fPulseAmplitude, sinf(m_fPulseTime), m_fWaterDropAmplitude, sinf(m_fWaterDropTime),
        0.5f / desc.Width, 0.5f / desc.Height, 1.f / dropDesc.Width, 1.f / dropDesc.Height };
    if (FAILED(pDevice->SetPixelShaderConstantF(0, constants, 2))) return E_FAIL;
    return pDevice->SetTexture(1, m_pWaterDropTexture);
}
