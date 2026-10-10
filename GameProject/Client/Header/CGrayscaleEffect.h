#pragma once
#include "CPostEffect.h"

class CGrayscaleEffect final : public CPostEffect
{
public:
    CGrayscaleEffect();
    void Set_Strength(_float fStrength);

private:
    HRESULT Bind_Resources(LPDIRECT3DDEVICE9 pDevice, const D3DSURFACE_DESC& desc) override;
    _float m_fStrength = 1.f;
};
