#include "pch.h"
#include "CMG0_DancerLArm.h"

CMG0_DancerLArm::CMG0_DancerLArm(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMG0_DancerPart(pGraphicDev)
{
}

CMG0_DancerLArm::~CMG0_DancerLArm()
{
}

HRESULT CMG0_DancerLArm::Ready_GameObject()
{
    return CMG0_DancerPart::Ready_GameObject(L"Proto_Player_LArm_Vertex");
}

_float CMG0_DancerLArm::Get_PoseAngle(_int iPose)
{
    static const _float fAngle[MG_DANCE_POSE_COUNT] = { -170.f, -90.f, 0.f, 0.f };

    return fAngle[iPose];
}

_float CMG0_DancerLArm::Get_SitAngle()
{
    return MG_PILOT_ARM_ANGLE;
}

CMG0_DancerLArm* CMG0_DancerLArm::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMG0_DancerLArm* pPart = new CMG0_DancerLArm(pGraphicDev);

    if (FAILED(pPart->Ready_GameObject()))
    {
        Safe_Release(pPart);
        MSG_BOX("CMG0_DancerLArm Create Failed");
        return nullptr;
    }

    return pPart;
}

void CMG0_DancerLArm::Free()
{
    CMG0_DancerPart::Free();
}
