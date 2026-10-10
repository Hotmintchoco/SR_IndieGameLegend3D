#include "pch.h"
#include "CShaderEffectMgr.h"
#include "CUnderwaterEffect.h"
#include "CLavaEffect.h"
#include "CInvertEffect.h"

IMPLEMENT_SINGLETON(CShaderEffectMgr)

CShaderEffectMgr::CShaderEffectMgr()
{
    Register_PostEffect(POST_EFFECT::UNDERWATER, new CUnderwaterEffect);
    Register_PostEffect(POST_EFFECT::LAVA, new CLavaEffect);
    Register_PostEffect(POST_EFFECT::INVERT, new CInvertEffect);
}

CShaderEffectMgr::~CShaderEffectMgr()
{
    Free();
}
_bool CShaderEffectMgr::Register_PostEffect(POST_EFFECT eType, CPostEffect* pEffect)
{
    if (eType == POST_EFFECT::NONE || !pEffect || m_postEffects.count(eType)) return false;
    for (const auto& entry : m_postEffects)
        if (entry.second == pEffect) return false;
    m_postEffects.emplace(eType, pEffect);
    return true;
}

CPostEffect* CShaderEffectMgr::Get_PostEffect(POST_EFFECT eType) const
{
    auto iter = m_postEffects.find(eType);
    return iter == m_postEffects.end() ? nullptr : iter->second;
}

_bool CShaderEffectMgr::Set_PostEffect(POST_EFFECT eType)
{
    CPostEffect* pEffect = Get_PostEffect(eType);
    if (eType != POST_EFFECT::NONE && !pEffect) return false;
    if (eType != m_ePostEffect && pEffect) pEffect->Retry();
    m_ePostEffect = eType;
    return true;
}

void CShaderEffectMgr::Update_PostEffect(_float fTimeDelta)
{
    if (auto* pEffect = Get_PostEffect(m_ePostEffect)) pEffect->Update(fTimeDelta);
}

_bool CShaderEffectMgr::Begin_PostEffect(LPDIRECT3DDEVICE9 pDevice)
{
    if (m_pCapturedEffect) return false;
    CPostEffect* pEffect = Get_PostEffect(m_ePostEffect);
    if (!pEffect || !pEffect->Begin(pDevice)) return false;
    m_pCapturedEffect = pEffect;
    return true;
}

void CShaderEffectMgr::End_PostEffect(LPDIRECT3DDEVICE9 pDevice)
{
    if (!m_pCapturedEffect) return;
    m_pCapturedEffect->End(pDevice);
    m_pCapturedEffect = nullptr;
}

void CShaderEffectMgr::Free()
{
    for (auto& entry : m_postEffects) delete entry.second;
    m_postEffects.clear();
    m_pCapturedEffect = nullptr;

}
