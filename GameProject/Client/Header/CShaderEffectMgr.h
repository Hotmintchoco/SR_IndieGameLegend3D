#pragma once
#include "CBase.h"
#include "CPostEffect.h"

// Client-owned screen effects. The engine renderer only draws game objects.
class CShaderEffectMgr : public CBase
{
    DECLARE_SINGLETON(CShaderEffectMgr);

private:
    CShaderEffectMgr();
    ~CShaderEffectMgr() override;

public:
    // Ownership transfers only on successful registration.
    _bool Register_PostEffect(POST_EFFECT eType, CPostEffect* pEffect);
    CPostEffect* Get_PostEffect(POST_EFFECT eType) const;
    _bool Set_PostEffect(POST_EFFECT eType);
    POST_EFFECT Get_PostEffectType() const { return m_ePostEffect; }
    void Update_PostEffect(_float fTimeDelta);
    _bool Begin_PostEffect(LPDIRECT3DDEVICE9 pDevice);
    void End_PostEffect(LPDIRECT3DDEVICE9 pDevice);

private:
    void Free() override;
    std::map<POST_EFFECT, CPostEffect*> m_postEffects;
    POST_EFFECT m_ePostEffect = POST_EFFECT::NONE;
    CPostEffect* m_pCapturedEffect = nullptr;
};
