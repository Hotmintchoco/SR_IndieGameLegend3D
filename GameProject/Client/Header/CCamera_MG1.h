#pragma once
#include "CCamera.h"
#include "Client_Struct.h"

namespace Engine
{
    class CTransform;
}
class CCamera_MG1 : public CCamera
{
protected:
    explicit CCamera_MG1(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CCamera_MG1(const CCamera_MG1& rhs);
    virtual ~CCamera_MG1();

public:
    HRESULT		    Ready_GameObject(CTransform* pTarget);

    _int            Update_GameObject(_float fTimeDelta) override;
    void            LateUpdate_GameObject(_float fTimeDelta) override;
    void            Render_GameObject() {}

    void SetPseudoScale(float fScale);

    inline TBillBoardInfo GetBillBoardInfo() { return m_tBillBoardInfo; }

public:
    void            Set_Target(CTransform* pTarget) { m_pTarget = pTarget; }
    void            Set_EyeOffset(const _vec3& vOffset) { m_vEyeOffset = vOffset; }

    void            Set_Distance(_float fDistance) { m_fDistance = fDistance; }
    _float          Get_Distance() const { return m_fDistance; }

private:
    void            Mouse_Move();
    void            Follow_Target();
    void UpdateBillBoardInfo();

    /* 스프링 암 관련 */
    float CalculateSpringArmLength();

private:
    CTransform* m_pTarget = nullptr;

    /* 카메라 빌보드 */
    TBillBoardInfo m_tBillBoardInfo;

    /* 가짜 스케일 관련 */
    _vec3 m_vEyeOffsetRaw = _vec3{ 0.f, 1.f, 0.f };
    float m_fNearRaw = 0.1f;
    float m_fFarRaw = 1000.f;

    /* 스프링 암 관련 */
    float m_fNearPlaneMargin = 0.1f;

    // 실제 플레이어 Transform의 기준점에 맞춰 설정
    _vec3           m_vEyeOffset = m_vEyeOffsetRaw;
    _float          m_fDistance;

public:
    static CCamera_MG1* Create(LPDIRECT3DDEVICE9 pGraphicDev, CTransform* pTarget);

private:
    virtual void		Free() override;
};

