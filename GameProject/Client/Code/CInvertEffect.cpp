#include "pch.h"
#include "CInvertEffect.h"

CInvertEffect::CInvertEffect() : CPostEffect(L"Invert.hlsl") {}

void CInvertEffect::Set_Strength(_float fStrength)
{
    if (!std::isfinite(fStrength)) return;
    m_fStrength = max(0.f, min(fStrength, 1.f));
}

HRESULT CInvertEffect::Bind_Resources(LPDIRECT3DDEVICE9 pDevice, const D3DSURFACE_DESC& desc)
{
    const float constants[] = { m_fStrength, 0.f, 0.f, 0.f };
    return pDevice->SetPixelShaderConstantF(0, constants, 1);
}
