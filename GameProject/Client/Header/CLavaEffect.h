#pragma once
#include "CPostEffect.h"

class CLavaEffect final : public CPostEffect
{
public:
    CLavaEffect();
    ~CLavaEffect() override;
    void Update(_float fTimeDelta) override;
    void Set_HeatParameters(_float fStrength, _float fSpeed);
    void Set_TintStrength(_float fStrength);

private:
    HRESULT Ready_Resources(LPDIRECT3DDEVICE9 pDevice) override;
    HRESULT Bind_Resources(LPDIRECT3DDEVICE9 pDevice, const D3DSURFACE_DESC& desc) override;
    _float m_fTime = 0.f;
    _float m_fStrength = 0.01f;
    _float m_fSpeed = 0.6f;
    _float m_fTintStrength = 0.1f;
    LPDIRECT3DTEXTURE9 m_pHeatTexture = nullptr;
};

