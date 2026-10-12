#pragma once
#include "CGameObject.h"
namespace Engine
{
    class CTransform;
    class CTexture;
    class CRcTex;
    class CSphereCollider;
}

class CGemBox : public CGameObject
{
private:
    explicit CGemBox(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CGemBox() = default;

public:
    HRESULT             Ready_GameObject() override;
    _int                Update_GameObject(_float fTimeDelta) override;
    void                Render_GameObject() override;

public:
    void                OnCollisionEnter(COLLINFO info) override;
    void                OnCollisionStay(COLLINFO info) override;
    void                OnCollisionExit(COLLINFO info) override;

    static CGemBox*     Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    HRESULT     Add_Component();
    void        Receive_Gem(COLLINFO info);
    void        Update_GemHUD();

private:
    Engine::CTransform*         m_pTransformCom = nullptr;
    Engine::CTexture*           m_pTextureCom = nullptr;
    Engine::CTexture*           m_pGemTextureCom = nullptr;
    Engine::CTexture*           m_pNumberTextureCom = nullptr;
    Engine::CRcTex*             m_pBufferCom = nullptr;
    Engine::CSphereCollider*    m_pColliderCom = nullptr;

    _int                        m_iRemainingGems = 100;
    _float                      m_fDepositInterval = 0.05f;
    _float                      m_fDepositTime = 0.f;
    _float                      m_fFrameDelta = 0.f;
    _float                      m_fFrame = 0.f;
    _bool                       m_bStart = false;
    _bool                       m_bDepositedThisFrame = false;

private:
    void        Free() override;
};

