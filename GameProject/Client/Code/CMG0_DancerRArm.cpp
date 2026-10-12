#include "pch.h"
#include "CMG0_DancerRArm.h"

CMG0_DancerRArm::CMG0_DancerRArm(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMG0_DancerPart(pGraphicDev)
{
}

CMG0_DancerRArm::~CMG0_DancerRArm()
{
}

HRESULT CMG0_DancerRArm::Ready_GameObject()
{
    return CMG0_DancerPart::Ready_GameObject(L"Proto_Player_RArm_Vertex");
}

_float CMG0_DancerRArm::Get_PoseAngle(_int iPose)
{
    static const _float fAngle[MG_DANCE_POSE_COUNT] = { 0.f, 0.f, 170.f, 90.f };

    return fAngle[iPose];
}

_float CMG0_DancerRArm::Get_SitAngle()
{
    return MG_PILOT_ARM_ANGLE;
}

CMG0_DancerRArm* CMG0_DancerRArm::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMG0_DancerRArm* pPart = new CMG0_DancerRArm(pGraphicDev);

    if (FAILED(pPart->Ready_GameObject()))
    {
        Safe_Release(pPart);
        MSG_BOX("CMG0_DancerRArm Create Failed");
        return nullptr;
    }

    return pPart;
}

void CMG0_DancerRArm::Free()
{
    CMG0_DancerPart::Free();
}
