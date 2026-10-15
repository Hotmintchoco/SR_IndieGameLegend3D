#pragma once

#include "CCamera.h"
#include <functional>

namespace Engine
{
    class CTransform;
}

class COrthoCamera : public CCamera
{
protected:
    explicit COrthoCamera(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~COrthoCamera();

public:
    virtual HRESULT	Ready_GameObject() override;
    virtual _int Update_GameObject(_float fTimeDelta) override;
    virtual void LateUpdate_GameObject(_float fTimeDelta) override;
    virtual void Render_GameObject() {}

    virtual void Update_Matrices() {}
    virtual void Apply_Transform() {}

    void ToggleProjectionMode(const float fDuration);
    void UpdateView(const _vec3& vEye, const _vec3& vAt);
    void UpdateView(const _matrix& matView);
    /* onFinished : 끝까지 도달한 순간 한 번만 호출 (카메라 LateUpdate 도중 호출되므로 받는 쪽은 플래그만 세우는 걸 권장)
       재생 중에 다시 Orbit을 부르면 이전 콜백은 호출되지 않고 버려짐 */
    void Orbit(const _vec3& vStart, const _vec3& vEnd, const _vec3& vAt, const float fDuration, std::function<void()> onFinished = nullptr);
    inline bool IsOrbiting() const { return m_bOnViewAnimation; }

    inline bool IsPerspective() { return m_bIsPerspective; }

private:
    CTransform* m_pTransform = nullptr;
    bool m_bIsPerspective = true;

    /* 투영 행렬 애니메이션 */
    bool m_bOnProjAnimation = false;
    float m_fProjAnimationTime = 1.f;
    float m_fTimeAfterProjAnimation = 0.f;
    
    /* 뷰 행렬 애니메이션 */
    bool m_bOnViewAnimation = false;
    float m_fViewAnimationTime = 1.f;
    float m_fTimeAfterViewAnimation = 0.f;
    /* 공전 파라미터 */
    _vec3 m_vOrbitStartOnSphere{ 0.f, 0.f, 0.f };
    _vec3 m_vOrbitEndOnSphere{ 0.f, 0.f, 0.f };
    _vec3 m_vOrbitAt{ 0.f, 0.f, 0.f };
    std::function<void()> m_OrbitFinished;

    /* 직교 투영 파라미터 */
    float m_fWidth = 20.f * 3.f / 4.f;
    float m_fHeight = 15.f * 3.f / 4.f;
    _matrix m_matProjOrtho;
    _matrix m_matProjPersp;

public:
    static COrthoCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void Free() override;
};

