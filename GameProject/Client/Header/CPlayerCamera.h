#pragma once
#include "CCamera.h"

namespace Engine
{
    class CTransform;
}

class CPlayerCamera : public CCamera
{
protected:
    explicit CPlayerCamera(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CPlayerCamera(const CPlayerCamera& rhs);
    virtual ~CPlayerCamera();

public:
    HRESULT		    Ready_GameObject(CTransform* pTarget);

    _int            Update_GameObject(_float fTimeDelta) override;
    void            LateUpdate_GameObject(_float fTimeDelta) override;
    void            Render_GameObject() {}

public:
    void            Set_Target(CTransform* pTarget) { m_pTarget = pTarget; }
    void            Set_EyeOffset(const _vec3& vOffset) { m_vEyeOffset = vOffset; }

private:
    void            Mouse_Move();
    void            Follow_Target();

private:
    CTransform*     m_pTarget = nullptr;

    // 실제 플레이어 Transform의 기준점에 맞춰 설정
    _vec3           m_vEyeOffset = _vec3{0.f, 1.f, 0.f};

public:
    static CPlayerCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev, CTransform* pTarget);

private:
    virtual void		Free() override;
};

