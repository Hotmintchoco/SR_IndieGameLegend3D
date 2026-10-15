#pragma once
#include "CGameObject.h"

namespace Engine
{
    class CTransform;
    class CTexture;
    class CPlaneTex;
    class CBoxCollider;
}

class CChasm : public CGameObject
{
private:
    explicit CChasm(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CChasm() = default;

public:
    HRESULT Ready_GameObject() override;
    _int Update_GameObject(_float fTimeDelta) override;
    void Render_GameObject() override;
    void OnCollisionEnter(COLLINFO info) override;
    void OnCollisionStay(COLLINFO info) override;
    void OnCollisionExit(COLLINFO info) override;
    void Set_Placement(const _vec3& vFloorPos, _float fWidth, _float fDepth);
    static CChasm* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    HRESULT Add_Component();
    void Free() override;
    Engine::CTransform* m_pTransformCom = nullptr;
    Engine::CTexture* m_pTextureCom = nullptr;
    Engine::CPlaneTex* m_pBufferCom = nullptr;
    Engine::CBoxCollider* m_pColliderCom = nullptr;
    _bool m_bPlacementApplied = false;
};
