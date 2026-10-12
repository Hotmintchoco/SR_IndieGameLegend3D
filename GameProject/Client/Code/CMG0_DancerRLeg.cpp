#include "pch.h"
#include "CMG0_DancerRLeg.h"

CMG0_DancerRLeg::CMG0_DancerRLeg(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMG0_DancerPart(pGraphicDev)
{
}

CMG0_DancerRLeg::~CMG0_DancerRLeg()
{
}

HRESULT CMG0_DancerRLeg::Ready_GameObject()
{
    return CMG0_DancerPart::Ready_GameObject(L"Proto_Player_RLeg_Vertex");
}

_float CMG0_DancerRLeg::Get_PoseAngle(_int iPose)
{
    static const _float fAngle[MG_DANCE_POSE_COUNT] = { 25.f, 0.f, 0.f, 0.f };

    return fAngle[iPose];
}

_float CMG0_DancerRLeg::Get_SitAngle()
{
    return MG_PILOT_LEG_ANGLE;
}

CMG0_DancerRLeg* CMG0_DancerRLeg::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMG0_DancerRLeg* pPart = new CMG0_DancerRLeg(pGraphicDev);

    if (FAILED(pPart->Ready_GameObject()))
    {
        Safe_Release(pPart);
        MSG_BOX("CMG0_DancerRLeg Create Failed");
        return nullptr;
    }

    return pPart;
}

void CMG0_DancerRLeg::Free()
{
    CMG0_DancerPart::Free();
}
