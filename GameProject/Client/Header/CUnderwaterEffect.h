#pragma once
#include "CPostEffect.h"

class CUnderwaterEffect final : public CPostEffect
{
public:
    CUnderwaterEffect();
    ~CUnderwaterEffect() override;
    void Update(_float fTimeDelta) override;
    void Set_PulseParameters(_float fStrength, _float fSpeed);
    void Set_WaterDropParameters(_float fStrength, _float fSpeed);

private:
    HRESULT Ready_Resources(LPDIRECT3DDEVICE9 pDevice) override;
    HRESULT Bind_Resources(LPDIRECT3DDEVICE9 pDevice, const D3DSURFACE_DESC& desc) override;
    _float m_fPulseTime = 0.f;
    _float m_fPulseAmplitude = 0.12f;
    _float m_fPulseSpeed = 1.5f;
    _float m_fWaterDropTime = 0.f;
    _float m_fWaterDropAmplitude = 0.04f;
    _float m_fWaterDropSpeed = 2.f;
    LPDIRECT3DTEXTURE9 m_pWaterDropTexture = nullptr;
};

