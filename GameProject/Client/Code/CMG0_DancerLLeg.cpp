#include "pch.h"
#include "CMG0_DancerLLeg.h"

CMG0_DancerLLeg::CMG0_DancerLLeg(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMG0_DancerPart(pGraphicDev)
{
}

CMG0_DancerLLeg::~CMG0_DancerLLeg()
{
}

HRESULT CMG0_DancerLLeg::Ready_GameObject()
{
    return CMG0_DancerPart::Ready_GameObject(L"Proto_Player_LLeg_Vertex");
}

_float CMG0_DancerLLeg::Get_PoseAngle(_int iPose)
{
    static const _float fAngle[MG_DANCE_POSE_COUNT] = { 0.f, 0.f, -25.f, 0.f };

    return fAngle[iPose];
}

_float CMG0_DancerLLeg::Get_SitAngle()
{
    return MG_PILOT_LEG_ANGLE;
}

CMG0_DancerLLeg* CMG0_DancerLLeg::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMG0_DancerLLeg* pPart = new CMG0_DancerLLeg(pGraphicDev);

    if (FAILED(pPart->Ready_GameObject()))
    {
        Safe_Release(pPart);
        MSG_BOX("CMG0_DancerLLeg Create Failed");
        return nullptr;
    }

    return pPart;
}

void CMG0_DancerLLeg::Free()
{
    CMG0_DancerPart::Free();
}
