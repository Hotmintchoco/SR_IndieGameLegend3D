#pragma once
#include "Engine_Define.h"

enum class POST_EFFECT { NONE, UNDERWATER, LAVA, INVERT };

// One effect owns its shader and scene capture. CShaderEffectMgr owns the effect.
class CPostEffect
{
public:
    explicit CPostEffect(const wchar_t* pShaderFile);
    virtual ~CPostEffect();
    CPostEffect(const CPostEffect&) = delete;
    CPostEffect& operator=(const CPostEffect&) = delete;

    virtual void Update(_float fTimeDelta) {}
    _bool Begin(LPDIRECT3DDEVICE9 pDevice);
    void End(LPDIRECT3DDEVICE9 pDevice);
    void Retry() { m_bFailed = false; }

protected:
    virtual HRESULT Ready_Resources(LPDIRECT3DDEVICE9 pDevice) { return S_OK; }
    virtual HRESULT Bind_Resources(LPDIRECT3DDEVICE9 pDevice, const D3DSURFACE_DESC& desc) = 0;
    static std::wstring ResourcePath(const wchar_t* pFile);

private:
    HRESULT Ready(LPDIRECT3DDEVICE9 pDevice, const D3DSURFACE_DESC& desc);
    void Release();
    std::wstring m_shaderFile;
    _bool m_bFailed = false;
    LPDIRECT3DTEXTURE9 m_pSceneTexture = nullptr;
    LPDIRECT3DSURFACE9 m_pSceneSurface = nullptr;
    LPDIRECT3DSURFACE9 m_pOutput = nullptr;
    LPDIRECT3DPIXELSHADER9 m_pShader = nullptr;
    D3DVIEWPORT9 m_viewport{};
};

